#define DEBUG

#include "../../dps_slave.h"
#include "../../common/can_mex/info_req.h"
#include "../../common/can_mex/variable.h"

#include <stdint.h>
#include <stdio.h>

#define Color_Red "\33[31m" 
#define Color_Green "\33[32m" 
#define Color_end "\33[0m" // To flush out prev settings

#define PASSED(str) printf(Color_Green str Color_end "\n"); passed++;
#define FAILED(str) printf(Color_Red str Color_end "\n"); failed++;

uint8_t passed = 0;
uint8_t failed = 0;

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
double ddata= 0;

typedef struct{
    uint8_t buffer[24];
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
    monitor_var(u64, dps_monitor_var_uint64_t, &u64);

    monitor_var(s8, dps_monitor_var_int8_t, &s8);
    monitor_var(s16, dps_monitor_var_int16_t, &s16);
    monitor_var(s32, dps_monitor_var_int32_t, &s32);
    monitor_var(s64, dps_monitor_var_int64_t, &s64);

    monitor_var(fdata, dps_monitor_var_float_t, &fdata);
    monitor_var(ddata, dps_monitor_var_double_t, &ddata);

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
    //TODO: unimplemented
    printf("to implement\n");
    return 0;
}

int check_update_var(){
#define update_var(VID,VALUE)  \
    {\
        VariableModify var ={\
            .full_data.value[0] = VALUE,\
            .full_data.obj_id.data_id= VID,\
            .full_data.obj_id.board_id= BOARD_ID,\
        };\
        CanMessage mex ={\
            .id = DPS_CAN_MESSAGE_ID,\
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,\
            .dps_payload.data = var.raw_data,\
        };\
        if (dps_check_can_command_recv(&mex)){\
            FAILED("update mex not recognized");\
            return -1;\
        }\
        PASSED("update mex recognized");\
    }

    update_var(0, 1);
    update_var(1, 6);
    update_var(2, 3);
    update_var(3, 3);
    update_var(4, 5);

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
        .full_data.obj_id.board_id = 1,
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

    printf("========================================\n");
    printf("passed %d\n",passed);
    printf("failed %d\n",failed);
    printf("========================================\n");

    return 0;
}
