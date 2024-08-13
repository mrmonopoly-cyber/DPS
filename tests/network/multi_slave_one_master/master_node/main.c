#include "../../../../dps_master.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>
#include <string.h>

#include <pthread.h>

int can_socket = -1;

int send_f_can(CanMessage* mex){
    struct can_frame f = {
        .can_dlc = mex->dlc,
        .can_id = mex->id,
    };
    memcpy(f.data, mex->rawMex.raw_buffer,mex->dlc);
    return can_send_frame(can_socket, &f);
}

void* check_incomming_message(void* args){
    struct can_frame mex_lib = {0};
    while(1){
        if(!can_recv_frame(can_socket, &mex_lib)){
            CanMessage mex = {
                .dlc = mex_lib.can_dlc,
                .id = mex_lib.can_id,
            };
            memcpy(mex.rawMex.raw_buffer, mex_lib.data, mex_lib.can_dlc);
            dps_master_check_mex_recv(&mex);
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
    dps_master_init(send_f_can);

    pthread_t new_thread = 1;
    pthread_create(&new_thread, NULL, check_incomming_message, NULL);
    

    pthread_join(new_thread, NULL);
    return 0;
}
