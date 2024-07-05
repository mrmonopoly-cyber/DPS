#include "dps_slave.h"
#include "lib/c_vector/c_vector.h"

#include <stddef.h>
#include <string.h>

//private
#define CHEK_INIT(r)   if (!monitor.vars || !monitor.comm){return r;}
#define ID_TYPE uint8_t

static ID_TYPE id_generator = 0;

enum MEX_TYPE{
    VAR,
    COM,
};

typedef struct dps_var_int {
    ID_TYPE id_data;
    dps_var var;
}dps_var_int;

typedef struct dps{
    c_vector* vars;
    c_vector* comm;
    can_send send_f;
    uint8_t board_id;
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

static void dummy_free(void* ele){}
static void dummy_print(const void* ele){}


//public
//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
//board_id: unique id of the board in the network
void dps_init(can_send send_f, uint8_t board_id)
{
    if (send_f == NULL) {
        return;
    }

    struct c_vector_input_init args_vars = {
        .print_fun = dummy_print,
        .free_fun = dummy_free,
        .ele_size = sizeof(dps_var_int),
        .capacity = 8,
        .found_f = found_dps_var,
    };

    struct c_vector_input_init args_comm = {
        .print_fun = dummy_print,
        .free_fun = dummy_free,
        .ele_size = sizeof(dps_command),
        .capacity = 8,
        .found_f = found_dps_com,
    };

    monitor.vars = c_vector_init(&args_vars);
    monitor.comm = c_vector_init(&args_comm);
    monitor.send_f = send_f;
    monitor.board_id = board_id;
}

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var)
{
    CHEK_INIT();

    struct dps_var_int new_var = {
        .var = *var,
        .id_data = id_generator,
    };
    id_generator++;
    c_vector_push(&monitor.vars, &new_var);
    
}

//INFO: tell to dps a dps_command the board can receive 
void dps_monitor_command(dps_command* comm)
{
    CHEK_INIT();

    c_vector_push(&monitor.comm, comm);
}

//BUFFER_DATA_VAR[8]:   BUFFER_DATA_COM[8]:     BUFFER_DATA_VARS[]:
//[0] = board_id        [0] = board_id          [0] = board_id 
//[1] = mex_type        [1] = mex_type          [1] = id_data
//[2] = id_data         [2,3] = id_can_com      [2,7] = data
//[3,7] = name          [4] = min               
//                      [5] = max
//                      [6,7] = name
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
        .data[0] = monitor.board_id,
        .mex_size = CAN_MAX_DATA_SIZE,
    };

    switch (mex->id.full_id) {
        case INFO:
            for (int i=0; i<var_vec_size; i++) {
                data_var_ptr = c_vector_get_at_index(monitor.vars, i);
                can_mex.data[1] = VAR;
                can_mex.data[2] = data_var_ptr->id_data;
                memcpy(&can_mex.data[3], data_var_ptr->var.name, 5);
                monitor.send_f(&can_mex);
            }

            for (int i=0; i<com_vec_size; i++) {
                data_com_ptr =  c_vector_get_at_index(monitor.comm, i);
                can_mex.data[1] = COM;
                memcpy(&can_mex.data[2], data_com_ptr, 6);
                monitor.send_f(&can_mex);
            }
            return 1;
        case VARS:
            if(mex->data[0] == monitor.board_id && 
                    (data_var_ptr = c_vector_find(monitor.vars, &mex->data[1]))){
                memcpy(data_var_ptr->var.var_ptr, &mex->data[2], data_var_ptr->var.size);
            }
            return 1;
        default:
            return 0;
    }

    return 0;
}

void dps_print_var(){
    c_vector_to_string(monitor.vars);
}