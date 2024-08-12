#include <stdlib.h>
#define __DEBUG__
#include "../../dps_master.h"
#include "../../common/can_mex/board.h"
#include "../../common/can_mex/variable.h"

#include "../test_lib.h"

#include <string.h>
#include <stdint.h>

#define NUMBER_SLAVES 5

char boards[NUMBER_SLAVES][BOARD_NAME_LENGTH+1] =
{
    "SLAVE1",
    "SLAVE2",
    "SLAVE3",
    "SLAVE4",
    "SLAVE5",
};

int debug_send(CanMessage* mex){
    return 0;
}

int new_board_connection(){
    for (long unsigned int i =0; i < NUMBER_SLAVES; i++) {
        BoardName new_board_name ={
        };
        memcpy(new_board_name.full_data.name, boards[i], BOARD_NAME_LENGTH);
        CanMessage mex = {
            .id = DPS_CAN_MESSAGE_ID,
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
            .dps_payload = {
                .mext_type = {GET_BOARD_NAME},
                .data = new_board_name.raw_data,
            },
        };
        if(dps_master_check_mex_recv(&mex)){
            FAILED("failed saved new board");
            return -1;
        }
        PASSED("ok saved new board");
    }

    board_list_info* boards_l = dps_master_list_board();
    for (uint8_t i =0; i < boards_l->board_num; i++) {
        if (strcmp(boards_l->boards[i].name, boards[i])) {
            free(boards_l);
            FAILED("board saved different from imput");
            return -1;
        }
        PASSED("ok board saved from imput");
    }
    free(boards_l);

    return 0;
} 

int test_saved_var()
{
 
#define ADD_NEW_VAR(BOARD_ID,VAR,VAR_ID,SIGNED,FLOAT) \
    {\
        CanMessage mex = {\
            .id = DPS_CAN_MESSAGE_ID,\
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,\
        };\
        VariableInfoName new_var_name = {\
            .full_data.obj_id = {.data_id = VAR_ID, .board_id = BOARD_ID},\
            .full_data.name = #VAR,\
        };\
        mex.dps_payload.mext_type.type = VAR_NAME,\
        mex.dps_payload.data = new_var_name.raw_data;\
        if(dps_master_check_mex_recv(&mex)){\
            FAILED("var name mex not recognized");\
            return -1;\
        }\
        PASSED("var name mex recognized");\
        VariableInfoMetadata new_var_metadata = {\
            .full_data.obj_id = {.board_id = BOARD_ID, .data_id = VAR_ID},\
            .full_data.size = sizeof(VAR),\
            .full_data.float_num = FLOAT,\
            .full_data.signe_num = SIGNED,\
        };\
        mex.dps_payload.mext_type.type = VAR_METADATA;\
        mex.dps_payload.data = new_var_metadata.raw_data;\
        if(dps_master_check_mex_recv(&mex)){\
            FAILED("var metadata mex not recognized");\
            return -1;\
        }\
        PASSED("var metadata mex recognized");\
    }

    uint8_t s_0_0;
    int8_t s_0_1;
    ADD_NEW_VAR(0, s_0_0, 0, 0, 0);
    ADD_NEW_VAR(0, s_0_1, 1, 1, 0);

    float s_1_0;
    int32_t s_1_1;
    ADD_NEW_VAR(1, s_1_0, 0, 1, 1);
    ADD_NEW_VAR(1, s_1_1, 1, 1, 1);

    float s_2_0;
    int32_t s_2_1;
    int32_t s_2_2;
    int32_t s_2_3;
    ADD_NEW_VAR(2, s_2_0, 0, 1, 1);
    ADD_NEW_VAR(2, s_2_1, 1, 1, 0);
    ADD_NEW_VAR(2, s_2_2, 2, 1, 0);
    ADD_NEW_VAR(2, s_2_3, 3, 1, 0);

    {
        var_list_info* list= NULL;
        if(dps_master_list_vars(2, &list)){
            if (list) {
                if (list->board_num != 4) {
                    FAILED("error checking saved vars board 2, wrong total number");
                    goto free;
                }
                PASSED("checking saved vars board 1, correct total number");
                for (uint8_t i=0; i<list->board_num; i++) {
                    var_record* var = &list->vars[i];
                    if (var->metadata.full_data.obj_id.board_id != 2) {
                    
                    }
                    switch (i) {
                        case 0:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_2_0) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 1 ||
                                    strcmp(var->name, "s_2_0")) {
                                FAILED("saving var s_2_0 failed");
                                goto free;
                            }
                            PASSED("saving var s_2_0 ok");
                            break;

                        case 1:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_2_1) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_2_1")) {
                                FAILED("saving var s_2_1 failed");
                                goto free;
                            }
                            PASSED("saving var s_2_1 ok");
                            break;

                        case 2:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_2_2) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_2_2")) {
                                FAILED("saving var s_2_2 failed");
                                goto free;
                            }
                            PASSED("saving var s_2_2 ok");
                            break;

                        case 3:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_2_3) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_2_3")) {
                                FAILED("saving var s_2_3 failed");
                                goto free;
                            }
                            PASSED("saving var s_2_3 ok");
                            break;
                    }
                }
            }
        }
        free(list);
        return 0;
free:
        free(list);
        return -1;
    }

    {
        var_list_info* list= NULL;
        if(dps_master_list_vars(0, &list)){
            if (list) {
                if (list->board_num != 2) {
                    FAILED("error checking saved vars board 1, wrong total number");
                    goto  free_1;
                }
                PASSED("checking saved vars board 1, correct total number");
                for (uint8_t i=0; i<list->board_num; i++) {
                    var_record* var = &list->vars[i];
                    if (var->metadata.full_data.obj_id.board_id != 1) {
                    
                    }
                    switch (i) {
                        case 0:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_1_0) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 1 ||
                                    strcmp(var->name, "s_1_0")) {
                                FAILED("saving var s_1_0 failed");
                                goto free_1;
                            }
                            PASSED("saving var s_1_0 ok");
                            break;

                        case 1:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_1_1) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_1_1")) {
                                FAILED("saving var s_1_1 failed");
                                goto free_1;
                            }
                            PASSED("saving var s_1_1 ok");
                            break;
                    }
                }
            }
        }

        free(list);
        return 0;
free_1:
        free(list);
        return -1;
    }

    {
        var_list_info* list= NULL;
        if(dps_master_list_vars(1, &list)){
            if (list) {
                if (list->board_num != 2) {
                    FAILED("error checking saved vars board 2, wrong total number");
                    goto  free_2;
                }
                PASSED("checking saved vars board 1, correct total number");
                for (uint8_t i=0; i<list->board_num; i++) {
                    var_record* var = &list->vars[i];
                    if (var->metadata.full_data.obj_id.board_id != 0) {
                    
                    }
                    switch (i) {
                        case 0:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_0_0) ||
                                    var->metadata.full_data.signe_num != 0 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_0_0")) {
                                FAILED("saving var s_0_0 failed");
                                goto free_2;
                            }
                            PASSED("saving var s_0_0 ok");
                            break;

                        case 1:
                            if (var->metadata.full_data.obj_id.data_id != i ||
                                    var->metadata.full_data.size != sizeof(s_0_1) ||
                                    var->metadata.full_data.signe_num != 1 ||
                                    var->metadata.full_data.float_num != 0 ||
                                    strcmp(var->name, "s_0_1")) {
                                FAILED("saving var s_0_1 failed");
                                goto free_2;
                            }
                            PASSED("saving var s_0_1 ok");
                            break;
                    }
                }
            }
        }

        free(list);
        return 0;
free_2:
        free(list);
        return -1;
    }

    return 0;
}

int test_saved_com()
{
 
#define ADD_NEW_COM(BOARD_ID,COM,COM_ID,SIGNED,FLOAT, SIZE) \
    {\
        CanMessage mex = {\
            .id = DPS_CAN_MESSAGE_ID,\
            .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,\
        };\
        CommandInfoName new_com_name = {\
            .full_data.com_id = COM_ID,\
            .full_data.name = COM,\
        };\
        mex.dps_payload.mext_type.type = COM_NAME,\
        mex.dps_payload.data = new_com_name.raw_data;\
        if(dps_master_check_mex_recv(&mex)){\
            FAILED("com name mex not recognize " #COM);\
            return -1;\
        }\
        PASSED("com name mex recognized " #COM);\
        CommandInfoMetadata new_com_metadata = {\
            .full_data.com_id = COM_ID,\
            .full_data.size = SIZE,\
            .full_data.float_num = FLOAT,\
            .full_data.signe_num = SIGNED,\
            .full_data.board_id = BOARD_ID, \
        };\
        mex.dps_payload.mext_type.type = COM_METADATA;\
        mex.dps_payload.data = new_com_metadata.raw_data;\
        if(dps_master_check_mex_recv(&mex)){\
            FAILED("com metadata mex not recognized " #COM);\
            return -1;\
        }\
        PASSED("com metadata mex recognized " #COM);\
    }

    ADD_NEW_COM(0,"S0C1", 0x12, 0, 0,1);
    ADD_NEW_COM(0,"S0C2", 0x13, 0, 0,2);

    ADD_NEW_COM(1,"S1C1", 0x14, 1, 0,1);
    ADD_NEW_COM(1,"S1C2", 0x15, 1, 0,4);

    ADD_NEW_COM(2,"S2C1", 0x16, 1, 1,3);
    ADD_NEW_COM(2,"S2C2", 0x17, 1, 1,4);

    com_list_info* com_l = NULL;
    if(dps_master_list_coms(&com_l)){
        FAILED("empty commands list");
        free(com_l);
        return -1;
    }
    PASSED("command list not empty");

#define CHECK_COM(INDEX,NAME,COM_ID,SIGNED,FLOAT, SIZE) \
    {\
        com_info* com = &com_l->coms[INDEX];\
        if (strcmp(com->name, NAME) ||\
                com->metadata.full_data.com_id != COM_ID || \
                com->metadata.full_data.size != SIZE ||\
                com->metadata.full_data.float_num != FLOAT ||\
                com->metadata.full_data.signe_num != SIGNED \
                ){\
            FAILED("command saved not match " NAME);\
            goto free_f;\
        }\
        PASSED("command saved match " NAME);\
    }
    
//CHECK_COM(INDEX, NAME, COM_ID, SIGNED, FLOAT, MIN, MAX, SIZE)
    CHECK_COM(0,"S0C1", 0x12, 0, 0,1);
    CHECK_COM(1,"S0C2", 0x13, 0, 0,2);

    CHECK_COM(2,"S1C1", 0x14, 1, 0,1);
    CHECK_COM(3,"S1C2", 0x15, 1, 0,4);
              
    CHECK_COM(4,"S2C1", 0x16, 1, 1,3);
    CHECK_COM(5,"S2C2", 0x17, 1, 1,4);

    free(com_l);
    return 0;

free_f:
    free(com_l);
    return -1;
}

int test_update_var(){
#define UPDATE_VAR(BOARD_ID, VAR_ID, VAR) \
    if (dps_master_update_var(BOARD_ID, VAR_ID, &VAR, sizeof(VAR))) {\
        FAILED("send update var value failed " #VAR);\
        return -1;\
    }\
    PASSED("send update var value ok " #VAR);

    uint8_t s_0_0 = 15;
    int8_t s_0_1 = -6;
    UPDATE_VAR(0, 0, s_0_0);
    UPDATE_VAR(0, 1, s_0_1);
    
    float s_1_0 = 5.3f;
    int32_t s_1_1 = -80;
    UPDATE_VAR(1, 0, s_1_0);
    UPDATE_VAR(1, 1, s_1_1);

    float s_2_0 = -4.3f;
    int32_t s_2_1 = -34;
    int32_t s_2_2 = 59;
    int32_t s_2_3 = 22;
    UPDATE_VAR(2, 0, s_2_0);
    UPDATE_VAR(2, 1, s_2_1);
    UPDATE_VAR(2, 2, s_2_2);
    UPDATE_VAR(2, 3, s_2_3);

    return 0;
}

int test_send_com(){
#define SEND_COM_C(ID,VAR)\
    if(dps_master_send_command(ID, &VAR, sizeof(VAR))){\
        FAILED("send command correct failed " #VAR);\
        return -1;\
    }\
    PASSED("send command correct ok" #VAR);

#define SEND_COM_F(ID,VAR)\
    if(!dps_master_send_command(ID, &VAR, sizeof(VAR))){\
        FAILED("send wrong command prevented failed " #VAR);\
        return -1;\
    }\
    PASSED("send wrong command prevented ok" #VAR);

    uint16_t com_0_f = 15;SEND_COM_F(18, com_0_f);
    uint8_t com_0_c = 3;SEND_COM_C(18, com_0_c);

    uint32_t com_1_f = 50 ;SEND_COM_F(19, com_1_f);
    uint16_t com_1_c = 40 ;SEND_COM_C(19, com_1_c);

    double com_2_f = -50;SEND_COM_F(23, com_2_f);
    float com_2_c = -3;SEND_COM_C(23, com_2_c);

    return 0;
}

int run_test(){
    if(dps_master_init(debug_send)){
        FAILED("failed init master");
        return -1;
    }
    PASSED("ok init master");


    if(dps_master_new_connection()){
        FAILED("failed sending new connection request");
        return -1;
    }
    PASSED("ok sending new connection request");

    if(new_board_connection()){
        FAILED("failed connection saving of new boards");
        return -1;
    }
    PASSED("ok connection saving of new boards");

    if (test_saved_var()) {
        FAILED("failed test saved var");
        return -1;
    }
    PASSED("ok test saved var");

    if (test_saved_com()) {
        FAILED("failed test saved com");
        return -1;
    }
    PASSED("ok test saved com");


    if (test_update_var()) {
        FAILED("update var failed");
        return -1;
    }
    PASSED("ok test update var");

    if (test_send_com()) {
        FAILED("send coms failed");
        return -1;
    }
    PASSED("send coms ok");

    return 0;
}

int main(void)
{
    run_test();
    print_SCORE();

    dps_master_print_boards();
    dps_master_print_coms();
    dps_master_print_vars();

    return 0;
}
