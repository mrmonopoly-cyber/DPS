#include <stdint.h>
#include <string.h>
#include <unistd.h>
#define DEBUG

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
        if(dps_master_check_mex_recv(&dps_mex)){
            FAILED("message not recognized");
        }
        PASSED("message recognized");
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
        strcmp(boards->boards[0].name, "SLAVE") ||
        boards->boards[0].id != 0){
        FAILED("finded slave differ from what exepected");
        return -1;
    }
    PASSED("finded slave is what exepected");
    return 0;
}

int req_info(){
    if(dps_master_request_info_board(0, REQ_COM | REQ_VAR)){
        FAILED("requesting info board failed");
    }
    PASSED("requesting info board ok");
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

    if(req_info()){
        FAILED("requiring info failed");
        return;
    }
    PASSED("requiring info ok");

    sleep(3);
    if (dps_master_refresh_value_var(0, 1)) {
        FAILED("failed refresh value var");
        return;
    }
    PASSED("refresh value var ok");

    sleep(3);
    var_record o_var;
    if(dps_master_get_value_var(0, 1, &o_var)){
        FAILED("failed get initial value var");
        return;
    }
    PASSED("get initial value var ok");

    uint16_t expt = 2;
    if (memcmp(o_var.value,&expt, sizeof(expt))) {
        FAILED("getted value are different than expected");
        return;
    }
    PASSED("getted value are ok");

    int16_t new = -20;
    if (dps_master_update_var(0, 4, &new, sizeof(new))) {
        FAILED("remote update value failed");
        return;
    }
    PASSED("remote update value ok");

    sleep(3);
    if (dps_master_refresh_value_var(0, 4)) {
        FAILED("failed refresh value var");
        return;
    }
    PASSED("refresh value var ok");

    sleep(3);
    var_record o_var_1;
    if(dps_master_get_value_var(0, 4, &o_var_1)){
        FAILED("failed get initial value var");
        return;
    }
    PASSED("get initial value var ok");

    if (memcmp(o_var_1.value,&new, sizeof(new))) {
        FAILED("getted value are different than expected");
        return;
    }
    PASSED("getted value are ok");
}


int main(int argc,char **argv)
{
    if (!argv[1]) {
        return -1;
    }
    can_socket = can_init(argv[1]);
    if (can_socket < 0) {
        printf("error init can\n");
        return -1;
    }
    dps_master_init(send_f_can);

    thrd_t th;
    thrd_create(&th, check_input_mex, NULL);

    run_test();

    sleep(3);

    dps_master_print_boards();
    dps_master_print_vars();
    dps_master_print_coms();

    print_SCORE();

    thrd_join(th, NULL);
    return 0;
}
