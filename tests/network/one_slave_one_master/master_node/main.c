#include <string.h>
#include <unistd.h>
#define __DEBUG__

#include "../../../../dps_master.h"
#include "../../can_lib/canlib.h"
#include "../../../test_lib.h"
#include <stdio.h>

#include <stdlib.h>
#include <threads.h>

int can_socket = -1;

int check_input_mex(void* args)
{
    while (1) {
        struct can_frame frame;
        CanMessage dps_mex;
        if(can_recv_frame(can_socket, &frame)){
            return EXIT_FAILURE;
        }

        dps_mex.id = frame.can_id;
        dps_mex.dlc = frame.can_dlc;
        memcpy(dps_mex.rawMex.raw_buffer, frame.data, frame.can_dlc);
        dps_master_check_mex_recv(&dps_mex);
        printf("message\n");
    }
}

int send_f_can(CanMessage* mex)
{
    struct can_frame frame = {
        .can_dlc = mex->dlc,
        .can_id = mex->id,
    };
    memcpy(frame.data, mex->rawMex.raw_buffer, mex->dlc);
    if(can_send_frame(can_socket, &frame)){
        printf("failed send");
    }
    return 0;
}

int get_boards(){
    sleep(1);
    board_list_info* boards = dps_master_list_board();
    if (!boards) {
        FAILED("failed get list of boards, empty");
        return -1;
    }
    if (boards->board_num != 1 ||
        strcmp(boards->boards[0].name, "SLAVE1") ||
        boards->boards[0].id != 0){
        FAILED("finded slave differ from what exepected");
        return -1;
    }
    PASSED("finded slave is what exepected");
    return 0;
}

void run_test(){
    if(dps_master_new_connection()){
        FAILED("failed sending new connection request");
        return;
    }
    PASSED("passed sending new connection request");

    if (get_boards()) {
        FAILED("failed get boards");
        return;
    }
    PASSED("ok getting boards");

}


int main(void)
{
    can_socket = can_init("test_can_dps");
    if (can_socket < 0) {
        printf("error init can\n");
        return -1;
    }
    dps_master_init(send_f_can);

    thrd_t th;
    thrd_create(&th, check_input_mex, NULL);

    run_test();

    thrd_join(th, NULL);
    return 0;
}
