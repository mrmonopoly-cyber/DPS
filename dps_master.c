#include "dps_master.h"
#include "dps_common.h"
#include "lib/c_vector/c_vector.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct board_data {
    uint8_t board_id;
    c_vector* vars;
    char board_name[BOARD_NAME_SIZE];
}board_data;

typedef struct dps {
    c_vector* board_vector;
    c_vector* coms;
    can_send send_f;
}dps;

static dps monitor;

static int found_board(const void* l_ele, const void* key){
    const board_data* board = (board_data *) l_ele;
    const uint8_t board_id = *(uint8_t *) key;

    return board->board_id == board_id;
}

static void print_var(const void* l_ele){
    const struct var_info_slave* var = (struct var_info_slave*) l_ele;
    printf("var info: data id %d\n",var->id_data);
    printf("var info: data size %d\n",var->data_size);
    printf("var info: data name %s\n",var->name);
}

static void print_com(const void* l_ele){
    const struct com_info_slave* com = (struct com_info_slave*) l_ele;
    printf("com info: com id %d\n",com->id_can_com);
    printf("com info: data name %s\n",com->name);
}

static void print_board(const void* l_ele){
    const board_data* board = (board_data *) l_ele;
    printf("board info: board id %d\n",board->board_id);
    printf("board info: board name %s\n",board->board_name);
    c_vector_to_string(board->vars);
    c_vector_to_string(monitor.coms);
}

static int found_var(const void* l_ele, const void* key){
    const struct var_info_slave* var= (struct var_info_slave *) l_ele;
    const uint8_t var_id= *(uint8_t *) key;

    return var->id_data == var_id;
}

static int found_com(const void* l_ele, const void* key){
    const struct com_info_slave* com= (struct com_info_slave*) l_ele;
    const uint16_t com_id = *(uint16_t *) key;

    return com->id_can_com == com_id;
}

static void dummy_free(void* e){}
static void dummy_print(const void* e){}

#define CHECK_INIT(r) if(!monitor.board_vector || !monitor.send_f) {\
    printf("failed init master\n"); \
    return r;\
}

//INFO: init the master
//send_f : function needed to send a can message
void dps_master_init(can_send send_f)
{
    struct c_vector_input_init args = {
        .found_f = found_board,
        .capacity = 8,
        .ele_size = sizeof(board_data),
        .free_fun = dummy_free,
        .print_fun = print_board,
    };

    struct c_vector_input_init com_args = {
        .capacity = 8,
        .ele_size = sizeof(struct com_info_slave),
        .free_fun = dummy_free,
        .print_fun = print_com,
        .found_f = found_com,
    };

    monitor.board_vector = c_vector_init(&args);
    monitor.coms = c_vector_init(&com_args);
    monitor.send_f = send_f;
}

//INFO: refresh the current set of variable sending a message to all the board 
//in the network
void dps_master_refresh(){
    CHECK_INIT();

    can_message can_mex ={
        .id = {INFO},
        .data = {0},
    };

    monitor.send_f(&can_mex);
}

//INFO: get a vector with all the BOARDS and their id
//pointer to free when no more needed
const board_info* dps_master_board_list()
{
    CHECK_INIT(NULL);

    uint8_t board_num = c_vector_length(monitor.board_vector);
    board_info* b_inf = calloc(board_num,sizeof(*b_inf));
    board_data* board = NULL;

    for (int i =0; i<board_num; i++) {
        board = c_vector_get_at_index(monitor.board_vector, i);
        b_inf[i].board_id = board->board_id;
        b_inf[i].board_name = board->board_name;
    }

    return b_inf;
}

//INFO: fetch the data of a specific board
//DO NOT FREE THE POINTER
const c_vector* dps_master_board_info(const uint8_t board_id,const enum DATA_BOARD ty_data)
{
    CHECK_INIT(NULL);
    const board_data* board = NULL;

    switch (ty_data) {
        case BOARD_VAR:
            board = c_vector_find(monitor.board_vector, &board_id);
            return board->vars;
            break;
        case BOARD_COM:
            return monitor.coms;
            break;
        default:
            return NULL;
    }
}
 
//INFO: send a mex to a specific board to updated a variable of the board
void dps_master_update(const uint8_t board_id, const uint8_t data_id, const void* value)
{
    CHECK_INIT();
    can_message var_update_mex;
    board_data* board_info= NULL;
    struct var_info_slave* var_info= NULL;

    if (!(board_info = c_vector_find(monitor.board_vector, &board_id))){
        return;
    }

    if (!(var_info = c_vector_find(board_info->vars, &data_id))) {
        return;
    }

    var_update_mex.id.full_id = VARS;
    var_update_mex.upd_master.board_id = board_id;
    var_update_mex.upd_master.id_data = data_id;
    var_update_mex.mex_size = CAN_MAX_DATA_SIZE;
    memcpy(var_update_mex.upd_master.value, value, var_info->data_size);

    monitor.send_f(&var_update_mex);

}

//INFO: send a command in the system with a given value
uint8_t dps_master_send_command(const can_id id_comm,const uint8_t value[CAN_MAX_DATA_SIZE])
{
    CHECK_INIT(1);

    can_message com_mex;
    board_data* board_info= NULL;
    struct com_info_slave* com_info= NULL;

    if( !(com_info = c_vector_find(monitor.coms, &id_comm))){
        return 3;
    }

    com_mex.id = id_comm;
    com_mex.mex_size = CAN_MAX_DATA_SIZE;   //FIX: add size of data of command in info
    memcpy(com_mex.data, value, CAN_MAX_DATA_SIZE);

    monitor.send_f(&com_mex);

    return 0;
}

//INFO: check if a given can message is of the belong to the library
//if it's the case it use it and return 1
//else return 0
uint8_t dps_master_check_can_mex_recv(const can_message* mex)
{
    CHECK_INIT(0);
    board_data board_info;
    board_data* board_info_ptr = NULL;
    struct var_info_slave new_var;
    struct com_info_slave new_com;
    struct com_info_slave* new_com_ptr;

    struct c_vector_input_init var_args = {
        .capacity = 8,
        .ele_size = sizeof(struct var_info_slave),
        .free_fun = dummy_free,
        .print_fun = print_var,
        .found_f = found_var,
    };


    switch (mex->id.full_id) {
        case RESP:
            switch (mex->info.mex_type) {
                case BRD:
                    if((board_info_ptr =  c_vector_find(monitor.board_vector, &mex->info.board_id))){
                        printf("board with id %d already present:\n \
                                saved: %s\n \
                                given: %s\n", 
                                mex->info.board_id, 
                                board_info_ptr->board_name,
                                mex->info.board_slave.name);
                        return -1;
                    }

                    board_info.board_id = mex->info.board_id;
                    memcpy(board_info.board_name, mex->info.board_slave.name, 
                            sizeof(board_info.board_name));
                    board_info.vars = c_vector_init(&var_args);
                    c_vector_push(&monitor.board_vector, &board_info);
                    printf("saving board %s\t%d\n",
                            mex->info.board_slave.name,
                            mex->info.board_id);
                    return 1;
                case VAR:
                    if ((board_info_ptr = c_vector_find(monitor.board_vector, 
                                    &mex->info.board_id))){
                        new_var = mex->info.var_slave;
                        new_var.name[VAR_NAME_SIZE-1] = '\0';
                        if (!c_vector_push(&board_info_ptr->vars, &new_var)){
                            printf("failed ");
                        };
                        printf("saving var %s\n",mex->info.var_slave.name);
                        return 1;
                    }
                    return 0;
                case COM:
                    new_com = mex->info.com_slave;
                    if((new_com_ptr =  c_vector_find(monitor.coms, &mex->info.com_slave.id_can_com))){
                        printf("com with id %d already present:\n \
                                saved: %s\n \
                                given: %s\n", 
                                new_com.id_can_com,
                                new_com_ptr->name,
                                new_com.name);
                        return -1;
                    }
                    if(!c_vector_push(&monitor.coms, &new_com)){
                        printf("failed ");
                    }
                    printf("saving com %s\n",mex->info.com_slave.name);
                    return 1;
            }
            break;
    }

    return 0;
}

void dps_master_print_board()
{
    c_vector_to_string(monitor.board_vector);
}
