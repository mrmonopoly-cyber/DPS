#include "dps.h"
#include "lib/c_vector/c_vector.h"

#include <stddef.h>
#include <string.h>

//private
#define CHEK_INIT(r)   if (monitor.data == NULL){ return r;}

static ID_TYPE id_generator = 0;

enum MEX_TYPE{
    VAR,
    COM,
};
typedef struct dps_data_int {
    enum MEX_TYPE mex_type;
    union{
        dps_var var;
        dps_command com;
    };
}dps_data_int;

typedef struct dps{
    c_vector* data;
    can_send send_f;
}dps;

static dps monitor;

static int found_dps_data(const void* list_ele,const void* out_ele){
    return *(ID_TYPE *)list_ele == *(ID_TYPE *) out_ele;
}

//public
//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
void dps_init(can_send send_f)
{
    if (send_f == NULL) {
        return;
    }

    struct c_vector_input_init args = {
        .print_fun = NULL,
        .free_fun = NULL,
        .ele_size = sizeof(dps_data_int),
        .capacity = 1,
        .found_f = found_dps_data,
    };

    monitor.data = c_vector_init(&args);
    monitor.send_f = send_f;
}

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var)
{
    CHEK_INIT();

    struct dps_data_int new_var = {
        .var = *var,
        .mex_type = VAR,
    };
    new_var.var.id_data = id_generator;
    id_generator++;
    c_vector_push(&monitor.data, &new_var);
}

//INFO: tell to dps a dps_command the board can receive 
void dps_board_command(dps_command* comm)
{
    CHEK_INIT();

    struct dps_data_int new_var = {
        .com = *comm,
        .mex_type = COM,
    };
    id_generator++;
    c_vector_push(&monitor.data, &new_var);
}

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t check_can_command_recv(can_message* mex)
{
    CHEK_INIT(0);

    uint16_t vec_size = c_vector_length(monitor.data);
    dps_data_int *data = NULL;
    can_message can_mex = {
        .id = INFO,
    };

    switch (mex->id) {
        case INFO:
            for (int i =0; i<vec_size; i++) {
                data = c_vector_get_at_index(monitor.data, i);
                switch (data->mex_type) {
                    case VAR:
                        can_mex.data[0] = data->var.id_data;
                        can_mex.data[1] = data->var.size;
                        memcpy(&can_mex.data[2], data->var.name, sizeof(data->var.name));
                        break;
                    case COM:
                        can_mex.data[0] = (data->com.id_can & 0xF0);
                        can_mex.data[1] = (data->com.id_can >> 1);
                        can_mex.data[2] = data->com.min;
                        can_mex.data[3] = data->com.max;
                        memcpy(&can_mex.data[4], data->com.name, sizeof(data->com.name));
                        break;
                }
                monitor.send_f(&can_mex);
            }
            return 1;
            break;
        case VARS:
            if (c_vector_find(monitor.data, &mex->data[0], data)){
                memcpy(data->var.var_ptr, &mex->data[1], mex->mex_size-1);
                return 1;
            }
            return 0;
            break;
        default:
            return 0;
            break;
    }
}
