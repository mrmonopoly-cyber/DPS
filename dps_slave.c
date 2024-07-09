#include "dps_slave.h"
#include "dps_common.h"
#include "lib/c_vector/c_vector.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

//private
#define CHEK_INIT(r)   if (!monitor.vars || !monitor.comm){\
    printf("failed init slave\n");\
    return r;\
}
#define ID_TYPE uint8_t

static ID_TYPE id_generator = 0;


typedef struct dps_var_int {
    ID_TYPE id_data;
    dps_var var;
}dps_var_int;

typedef struct dps{
    c_vector* vars;
    c_vector* comm;
    can_send send_f;
    uint8_t board_id;
    char board_name[BOARD_NAME_SIZE];
}dps;

static dps monitor;

static int found_dps_var(const void* list_ele,const void* out_ele){
    return *(ID_TYPE *)list_ele == *(ID_TYPE *) out_ele;
}

static int found_dps_com(const void* list_ele,const void* out_ele){
    dps_command* l_ele = (dps_command *) list_ele;
    can_id* f_id = (can_id *)out_ele;

    return l_ele->id_can.full_id == f_id->full_id;
}
static void var_print(const void* ele){
    dps_var_int* var = (dps_var_int *)ele;
    printf("var id: %d\n\
            var name: %s\n\
            var size: %d\n\
            var ptr: %ld\n",var->id_data,var->var.name,var->var.size,(long) var->var.var_ptr);
}

static void com_print(const void* ele){
    dps_command* com = (dps_command*)ele;
    printf("com id: %d\n\
            com name: %s\n",com->id_can.full_id,com->name);
}

static void dummy_free(void* ele){}
static void dummy_print(const void* ele){}



//public
//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
//board_id: unique id of the board in the network
void dps_init(can_send send_f, uint8_t board_id, char board_name[BOARD_NAME_SIZE])
{
    if (send_f == NULL) {
        return;
    }

    struct c_vector_input_init args_vars = {
        .print_fun = var_print,
        .free_fun = dummy_free,
        .ele_size = sizeof(dps_var_int),
        .capacity = 8,
        .found_f = found_dps_var,
    };

    struct c_vector_input_init args_comm = {
        .print_fun = com_print,
        .free_fun = dummy_free,
        .ele_size = sizeof(dps_command),
        .capacity = 8,
        .found_f = found_dps_com,
    };

    monitor.vars = c_vector_init(&args_vars);
    monitor.comm = c_vector_init(&args_comm);
    monitor.send_f = send_f;
    monitor.board_id = board_id;
    memcpy(monitor.board_name, board_name, sizeof(monitor.board_name));
}

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var)
{
    CHEK_INIT();

    struct dps_var_int new_var = {
        .var = *var,
        .id_data = id_generator,
    };
    new_var.var.name[VAR_MAX_NAME_SIZE_SLAVE-1] = '\0';
    id_generator++;
    c_vector_push(&monitor.vars, &new_var);
}

//INFO: tell to dps a dps_command the board can receive 
void dps_monitor_command(dps_command* comm)
{
    CHEK_INIT();

    c_vector_push(&monitor.comm, comm);
}

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t dps_check_can_command_recv(can_message* mex)
{
    CHEK_INIT(0);

    uint16_t var_vec_size = c_vector_length(monitor.vars);
    uint16_t com_vec_size = c_vector_length(monitor.comm);
    dps_var_int* data_var_ptr = NULL;
    dps_command* data_com_ptr = NULL;
    can_message can_mex = {
        .id = {RESP},
        .mex_size = CAN_MAX_DATA_SIZE,
    };

    switch (mex->id.full_id) {
        case INFO:
            can_mex.info.mex_type = BRD;
            can_mex.info.board_id = monitor.board_id;
            memcpy(can_mex.info.board_slave.name, monitor.board_name, 
                    sizeof(can_mex.info.board_slave.name));
            if (can_mex.info.board_slave.name[BOARD_NAME_SIZE-1] != '\0') {
                printf("WARNING, board name too long, setting last bit to \
                        the terminator\n");
                can_mex.info.board_slave.name[BOARD_NAME_SIZE-1] = '\0';
            }
            monitor.send_f(&can_mex);

            for (int i=0; i<var_vec_size; i++) {
                data_var_ptr = c_vector_get_at_index(monitor.vars, i);
                can_mex.info.mex_type = VAR;
                can_mex.info.var_slave.id_data = data_var_ptr->id_data;
                can_mex.info.var_slave.data_size = data_var_ptr->var.size;
                memcpy(can_mex.info.var_slave.name, data_var_ptr->var.name, 
                        sizeof(can_mex.info.var_slave.name));

                if (can_mex.info.var_slave.name[VAR_NAME_SIZE-1] != '\0') {
                    printf("WARNING, var name too long, setting last bit to \
                            the terminator\n");
                    can_mex.info.board_slave.name[BOARD_NAME_SIZE-1] = '\0';
                }
                monitor.send_f(&can_mex);
            }

            for (int i=0; i<com_vec_size; i++) {
                data_com_ptr =  c_vector_get_at_index(monitor.comm, i);
                can_mex.info.com_slave.id_can_com = data_com_ptr->id_can.full_id;
                can_mex.info.mex_type = COM;
                memcpy(can_mex.info.com_slave.name, data_com_ptr->name, 
                        sizeof(can_mex.info.com_slave.name));
                if (can_mex.info.com_slave.name[COM_NAME_SIZE-1] != '\0') {
                    printf("WARNING, com name too long, setting last bit to \
                            the terminator\n");
                    can_mex.info.board_slave.name[BOARD_NAME_SIZE-1] = '\0';
                }
                monitor.send_f(&can_mex);
            }
            return 1;
        case VARS:
            if(mex->upd_master.board_id == monitor.board_id && 
                    (data_var_ptr = c_vector_find(monitor.vars, &mex->upd_master.id_data))){
                memcpy(data_var_ptr->var.var_ptr, mex->upd_master.value, data_var_ptr->var.size);
            }
            return 1;
        default:
            return 0;
    }

    return 0;
}

void dps_print_slave(){
    CHEK_INIT();

    printf("board id: %d\nboard name: %s\n",monitor.board_id,monitor.board_name);
    c_vector_to_string(monitor.vars);
    c_vector_to_string(monitor.comm);
}
