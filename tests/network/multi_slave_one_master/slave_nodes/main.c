#include "../../../../dps_slave.h"
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
            dps_check_can_command_recv(&mex);
        }
    };
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("invalid input number %d\n\
                argv[1] = board_name of size <= 6\n\
                argv[2] = num of vars\n\
                argv[3] = num of coms\n",argc);
        return -1;
    }

    if (strlen(argv[2]) > 6){
        printf("invalid board name %s\n\
                max len: %ld\n \
                given: %ld\n",argv[2], NAME_MAX_SIZE ,strlen(argv[2]));
        return -2;
    }

    can_socket = can_init("test_can_dps");
    if (can_socket < 0) {
        printf("error init can\n");
        return -3;
    }

    char* board_name_orig = argv[1];
    uint8_t num_vars = atoi(argv[2]);
    uint8_t num_coms = atoi(argv[3]);

    BoardName board_name = {
    };
    memcpy(board_name.full_data.name, board_name_orig, NAME_MAX_SIZE);
    dps_init(send_f_can,&board_name);

    pthread_t new_thread = 1;
    pthread_create(&new_thread, NULL, check_incomming_message, NULL);


    pthread_join(new_thread, NULL);
    return 0;
}
