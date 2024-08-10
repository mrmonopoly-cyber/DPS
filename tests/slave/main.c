#include <assert.h>
#include <string.h>

#define DEBUG
#include "../../dps_slave.h"
#include "../../common/can_mex/info_req.h"
#include "../../common/can_mex/variable.h"

#include <stdint.h>

#include "../test_lib.h"

#define BOARD_ID 5

uint8_t u8 =0;
uint16_t u16 = 0;
uint32_t u32= 0;
uint64_t u64= 0;

int8_t s8 =0;
int16_t s16 = 0;
int32_t s32= 0;
int64_t s64= 0;

float fdata= 0;

typedef struct{
    uint8_t a;
    uint16_t b;
    uint8_t c;
}generic_type;

generic_type gen_t = {};

int check_monitor_var()
{
#define monitor_var(n,fun,macro_ptr) \
    {\
        VariableInfoPrimitiveType var_info= {\
            .name = #n,\
            .var_ptr = macro_ptr\
        };\
        if (fun (&var_info)){\
            FAILED("error saving var " #n); \
            return -2;\
        }\
        PASSED("check ok var " #n);\
    }\

    monitor_var(u8, dps_monitor_var_uint8_t, &u8);
    monitor_var(u16, dps_monitor_var_uint16_t, &u16);
    monitor_var(u32, dps_monitor_var_uint32_t, &u32);

    monitor_var(s8, dps_monitor_var_int8_t, &s8);
    monitor_var(s16, dps_monitor_var_int16_t, &s16);
    monitor_var(s32, dps_monitor_var_int32_t, &s32);

    monitor_var(fdata, dps_monitor_var_float_t, &fdata);

    VariableInfoGericType gen = {
        .name = "gen_t",
        .var_ptr = &gen_t,
        .size= sizeof(gen_t),
        .float_var = 0,
        .signd_var = 1,
    };

    if(dps_monitor_var_general(&gen)){
            FAILED("error saving var gen_t");
            return -2;
    }
    PASSED("check ok save var gen_t");

    return 0;
}

int check_monitor_com()
{
#define monitor_com(ID,DLC,MAX,MIN,NAME,FLOAT,SIGN) \
    {\
        CommandInfo com ={\
            .id = ID,\
            .dlc = DLC,\
            .max = MAX,\
            .min = MIN,\
            .name = NAME,\
            .float_num= FLOAT,\
            .signd_num = SIGN,\
        };\
        if(dps_monitor_command(&com)){\
            FAILED("monitor command save failed");\
            return -1;\
        }\
        PASSED("monitor command save success");\
    }

    monitor_com(15, 6, 50, 5, "COM_1S", 0, 0);
    monitor_com(20, 8, 50, 5, "COM_2S", 1, 1);

    return 0;
}

int check_update_var(){
#define update_var(VID,VALUE_T, VALUE_D)  \
    {\
        VALUE_T mod = VALUE_D;\
        VariableModify var ={\
            .full_data.obj_id.data_id= VID,\
            .full_data.obj_id.board_id= BOARD_ID,\
        };\
        memcpy(var.full_data.value, &mod, sizeof(mod));\
        CanMessage mex ={\
            .id = DPS_CAN_MESSAGE_ID,\
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,\
            .dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},\
            .dps_payload.data = var.raw_data,\
        };\
        if (dps_check_can_command_recv(&mex)){\
            FAILED("update mex not recognized");\
            return -1;\
        }\
        PASSED("update mex recognized");\
    }

    update_var(0, uint8_t, 1);
    update_var(1, uint16_t,2);
    update_var(2, uint32_t,3);

    update_var(3, int8_t,-1);
    update_var(4, int16_t,-2);
    update_var(5, int32_t,-3);

    {
        float new_value = 2.5f;
        VariableModify var ={
            .full_data.obj_id.data_id= 6,
            .full_data.obj_id.board_id= BOARD_ID,
        };
        mempcpy(var.full_data.value, &new_value, sizeof(new_value));
        CanMessage mex ={
            .id = DPS_CAN_MESSAGE_ID,
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
            .dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},\
            .dps_payload.data = var.raw_data,
        };
        if (dps_check_can_command_recv(&mex)){
            FAILED("update mex not recognized");
            return -1;
        }
        PASSED("update mex recognized");
    }

    {
        generic_type new_value = {
            .a = 4,
            .b = 12,
            .c = 24,
        };
        VariableModify var ={
            .full_data.obj_id.data_id= 7,
            .full_data.obj_id.board_id= BOARD_ID,
        };
        mempcpy(var.full_data.value, &new_value, sizeof(new_value));
        CanMessage mex ={
            .id = DPS_CAN_MESSAGE_ID,
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
            .dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},\
            .dps_payload.data = var.raw_data,
        };
        if (dps_check_can_command_recv(&mex)){
            FAILED("update mex not recognized");
            return -1;
        }
        PASSED("update mex recognized");
    }
    
#define check_variable(var,value_expected) \
    u8 == 1 ? PASSED("u8 updated correctly") : FAILED("u8 update failed");
    u16 == 2 ? PASSED("u16 updated correctly") : FAILED("u16 update failed");
    u32 == 3 ? PASSED("u32 updated correctly") : FAILED("u32 update failed");

    s8 == -1 ? PASSED("s8 updated correctly") : FAILED("s8 update failed");
    s16 == -2 ? PASSED("s16 updated correctly") : FAILED("s16 update failed");
    s32 == -3 ? PASSED("s32 updated correctly") : FAILED("s32 update failed");

    fdata == 2.5f ? PASSED("fdata updated correctly") : FAILED("fdata update failed");

    if(gen_t.a !=4 || gen_t.b != 12 || gen_t.c != 24){
        FAILED("generic type update failed");
        return -1;
    }
    PASSED("generic type update ok");
    return 0;
}

int check_link_req(){
    ReqInfo new_connection= {
        .full_data.info_t = NEW_CONNECTION,
    };

    CanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .dps_payload.mext_type = {GET_INFO},
        .dps_payload.data = new_connection.raw_data,
    };

    if(dps_check_can_command_recv(&mex)){
        FAILED("new connection message not recognized");
        return -1;
    }
    PASSED("new connection message recognized");
    return 0;
}

int check_id_assign(){
    AssignBoarId correct_assign = {
        .full_data.obj_id.board_id = BOARD_ID,
        .full_data.name = "TSLAVE",
    };

    AssignBoarId wrong_assign = {
        .full_data.obj_id.board_id = 2,
        .full_data.name = "WRONG",
    };
    {
        CanMessage mex = {
            .id = DPS_CAN_MESSAGE_ID,
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
            .dps_payload.mext_type = {SET_BOARD_ID},
            .dps_payload.data = correct_assign.raw_data,
        };
        if (dps_check_can_command_recv(&mex)){
            FAILED("can message for id assignment not recognized");
            return -1;
        }
        PASSED("can message for id assignment correct recognized");
        if (dps_get_id() != correct_assign.full_data.obj_id.board_id){
            FAILED("correct id to board not assigned");
            return -2;
        }
        PASSED("correct id to board assigned");
    }

    {
        CanMessage mex = {
            .id = DPS_CAN_MESSAGE_ID,
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
            .dps_payload.mext_type = {SET_BOARD_ID},
            .dps_payload.data = wrong_assign.raw_data,
        };
        if (dps_check_can_command_recv(&mex)){
            FAILED("can message for id assignment not recognized");
            return -1;
        }
        PASSED("can message for id assignment wrong recognized");
        if (dps_get_id() == wrong_assign.full_data.obj_id.board_id){
            FAILED("wrong id to board assigned");
            return -2;
        }
        PASSED("wrong id to board not assigned");
    }
    return 0;
}

int send(CanMessage* mex){
    return 0;
}

int run_test(){
    BoardName board_name = {
        .full_data.name = "TSLAVE",
    };

    if (dps_init(send,&board_name)){
        FAILED("failed init of slave");
        return -1;
    }
    PASSED("init completed");

    if(check_link_req()){
        FAILED("failed to receive link request");
        return -2;
    }

    if(check_id_assign()){
        FAILED("failed assign id board");
        return -3;
    }

    if (check_monitor_var()){
        FAILED("failed saving vars");
        return -4;
    }

    if (check_monitor_com()){
        FAILED("failed saving coms");
        return -5;
    }

    if (check_update_var()){
        FAILED("failed updating vars");
        return -6;;
    }
    return 0;
}

int main(void)
{
    run_test();
    print_SCORE();
    return 0;
}
