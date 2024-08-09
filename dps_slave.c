#include "dps_slave.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/mex_types.h"
#include "common/can_mex/board.h"
#include "common/can_mex/info_req.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"
#include "lib/c_vector/c_vector.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//private

struct slave_dps{
    char board_name[BOARD_NAME_LENGTH];
    can_send send_f;
    c_vector* vars;
    c_vector* coms;
    int8_t board_id;
};

struct var_internal{
    uint8_t var_id :4; 
    VariableInfoGericType data;
};

static struct slave_dps dps;
static uint8_t object_id_slave = 0;

static void dummy_fun(void* e){}

static int found_var(const void* list_ele, const void* key){
    struct var_internal* ele_l = (struct var_internal *) list_ele;
    uint8_t id = *(uint8_t *) key;

    return ele_l->var_id == id;
}

static void print_var(const void* ele){
    const struct var_internal* var = ele;
    printf("var_id :%d,",var->var_id);
    printf("var_name :%s\n",var->data.name);
}

static uint8_t new_id(){
    object_id_slave++;
    return  object_id_slave-1;
}

static int req_inf_exec(CanMessage* mex){
    ReqInfo data_mex = {
        .raw_data = mex->dps_payload.data,
    };
    switch (data_mex.full_data.info_t) {
        case NEW_CONNECTION:
            {
                BoardName resp_payload;
                memcpy(resp_payload.full_data.name, dps.board_name, 
                        BOARD_NAME_LENGTH);
                CanMessage mex = {
                    .id = DPS_CAN_MESSAGE_ID,
                    .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
                    .dps_payload.data = resp_payload.raw_data,
                };
                if(dps.send_f(&mex)){
                    return EXIT_FAILURE;
                }
                return EXIT_SUCCESS;
            }
            break;
        default:
            perror("not implemented");

    }
    return EXIT_FAILURE;
}

static int set_board_id_exec(CanMessage* mex){
    AssignBoarId board_payload = {
        .raw_data = mex->dps_payload.data,
    };
    if(!strncmp(dps.board_name, board_payload.full_data.name, BOARD_NAME_LENGTH)){
        dps.board_id = board_payload.full_data.obj_id.board_id;
    }
    return EXIT_SUCCESS;
}

static int set_var_value_exec(CanMessage* mex){
    VariableModify new_value = {
        .raw_data = mex->dps_payload.data,
    };

    if (new_value.full_data.obj_id.board_id == dps.board_id) {
        uint8_t var_id = new_value.full_data.obj_id.data_id;
        struct var_internal* saved_var = c_vector_find(dps.vars, &var_id);
        if (saved_var){
            mempcpy(saved_var->data.var_ptr, new_value.full_data.value, saved_var->data.size);
        }
    }

    return EXIT_SUCCESS;
}

//public
int dps_init(can_send send_f, BoardName* board_name)
{
    assert(send_f);
    assert(board_name);

    memcpy(dps.board_name, board_name->full_data.name, BOARD_NAME_LENGTH);
    dps.send_f = send_f;
    dps.board_id = -1;
    {
        struct c_vector_input_init args = {
            .capacity = 10,
            .ele_size = sizeof(struct var_internal),
            .free_fun =  dummy_fun,
            .print_fun = print_var,
            .found_f = found_var,
        };
        dps.vars = c_vector_init(&args);
        if (!dps.vars){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int dps_monitor_var_uint8_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(uint8_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 0,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
int dps_monitor_var_uint16_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(uint16_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 0,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
int dps_monitor_var_uint32_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(uint32_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 0,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int dps_monitor_var_int8_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(uint8_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 1,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS; 
}
int dps_monitor_var_int16_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(int16_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 1,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
int dps_monitor_var_int32_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(int32_t),
            .var_ptr = var_info->var_ptr,
            .float_var = 0,
            .signd_var = 1,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int dps_monitor_var_float_t(VariableInfoPrimitiveType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = {
            .size = sizeof(float),
            .var_ptr = var_info->var_ptr,
            .float_var = 1,
            .signd_var = 1,
        },
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//INFO: tell to dps to monitor a variable
int dps_monitor_var_general(VariableInfoGericType* var_info)
{
    assert(var_info);
    assert(var_info->var_ptr);
    assert(var_info->name);

    struct var_internal new_var = {
        .var_id = new_id(),
        .data = *var_info,
    };
    memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name));

    if (!c_vector_push(&dps.vars, &new_var)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//INFO: tell to dps a dps_command the board can receive 
int dps_monitor_command(CommandInfo* comm_name)
{
    return EXIT_FAILURE;
}

//INFO: check if a can message is for the dps and if it's the case it executes the message
int dps_check_can_command_recv(CanMessage* mex)
{
    assert(mex);

    if (mex->id == DPS_CAN_MESSAGE_ID){
        switch(mex->dps_payload.mext_type.type){
            case GET_INFO:
                return req_inf_exec(mex);
            case SET_BOARD_ID:
                return set_board_id_exec(mex);
            case SET_CURRENT_VAR_VALUE:
                return set_var_value_exec(mex);
        }
    }

    return EXIT_FAILURE;
}

int dps_get_id()
{
    return dps.board_id;
}

void dps_print_var()
{
    struct var_internal* var = NULL;
    uint len = c_vector_length(dps.vars);
    for (int i=0; i<len; i++) {
        var = c_vector_get_at_index(dps.vars, i);
        print_var(var);
    }
}
