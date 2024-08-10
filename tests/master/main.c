#include <stdlib.h>
#define __DEBUG__
#include "../../dps_master.h"
#include "../../common/can_mex/board.h"

#include "../test_lib.h"

#include <string.h>
#include <stdint.h>


char *boards[BOARD_NAME_LENGTH] =
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
    for (long unsigned int i =0; i < 5; i++) {
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

    return 0;
}

int main(void)
{
    run_test();
    print_SCORE();

    dps_master_print_boards();

    return 0;
}
