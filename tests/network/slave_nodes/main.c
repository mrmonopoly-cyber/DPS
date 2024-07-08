#include "../../../dps_slave.h"
#include "../can_lib/canlib.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

int can_socket = -1;

uint8_t send_f_can(can_message* mex){
    struct can_frame f = {
        .can_dlc = mex->mex_size,
        .can_id = mex->id.full_id,
    };
    memcpy(f.data, mex->data,mex->mex_size);
    return can_send_frame(can_socket, &f);
}

void* check_incomming_message(void* args){
    struct can_frame mex_lib = {0};
    struct can_frame* mex_lib_ptr = &mex_lib;
    can_message mex = {0};
    while(1){
        if(can_recv_frame(can_socket, &mex_lib_ptr)){
            mex.mex_size = mex_lib.can_dlc;
            mex.id.full_id = mex_lib.can_id;
            memcpy(mex.data, mex_lib.data, mex_lib.can_dlc);
            if(dps_check_can_command_recv(&mex)){
                printf("dps slave receive a message\n");
            }
            printf("receive a message\n");
        }
    };
    return NULL;
}

int main(void)
{
    can_socket = can_init("test_can_dps");
    if (can_socket < 0) {
        printf("error init can\n");
        return -1;
    }

    uint8_t board_id = 0;
    char board_name[6] = "SLAVE1";
    dps_init(send_f_can, board_id,board_name);

    pthread_t new_thread = 1;
    pthread_create(&new_thread, NULL, check_incomming_message, NULL);

    uint8_t gas = 0;
    float brk = 0;

    dps_var mon_var = {0};

    mon_var.var_ptr = &gas;
    mon_var.size = sizeof(gas);
    char gas_name[] = "GAS";
    memcpy(mon_var.name, gas_name, sizeof(gas_name));
    dps_monitor_var(&mon_var);

    mon_var.var_ptr = &brk;
    mon_var.size = sizeof(brk);
    char brk_name[] = "BRAKE";
    memcpy(mon_var.name, brk_name, sizeof(brk_name));
    dps_monitor_var(&mon_var);

    printf("var saved init completed\n");


    pthread_join(new_thread, NULL);
    return 0;
}
