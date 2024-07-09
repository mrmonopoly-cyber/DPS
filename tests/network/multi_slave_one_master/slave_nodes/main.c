#include "../../../../dps_slave.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

int can_socket = -1;

uint8_t send_f_can(can_message* mex){

    switch (mex->id.full_id) {
        case RESP:
            printf("sending info board id: %d\n", mex->info.board_id);
            switch (mex->info.mex_type) {
                case BRD:
                    printf("sending info board mex\n");
                    printf("sending info board name: %s\n", mex->info.board_slave.name);
                    break;
                case VAR:
                    printf("sending info var mex\n");
                    printf("sending info var data id: %d\n", mex->info.var_slave.id_data);
                    printf("sending info var data size: %d\n", mex->info.var_slave.data_size);
                    printf("sending info var data name: %s\n", mex->info.var_slave.name);
                    break;
                case COM:
                    printf("sending info com mex\n");
                    printf("sending info com data id: %d\n", mex->info.com_slave.id_can_com);
                    printf("sending info com data name: %s\n", mex->info.com_slave.name);
                    break;
            }
            printf("-----------------------------------------------\n");
    }

    struct can_frame f = {
        .can_dlc = mex->mex_size,
        .can_id = mex->id.full_id,
    };
    memcpy(f.data, mex->data,mex->mex_size);
    return can_send_frame(can_socket, &f);
}

void* check_incomming_message(void* args){
    struct can_frame mex_lib = {0};
    can_message mex = {0};
    while(1){
        if(!can_recv_frame(can_socket, &mex_lib)){
            mex.mex_size = mex_lib.can_dlc;
            mex.id.full_id = mex_lib.can_id;
            memcpy(mex.data, mex_lib.data, mex_lib.can_dlc);
            if(dps_check_can_command_recv(&mex)){
                switch (mex.id.full_id) {
                    case VARS:
                        printf("receved update var board id: %d\n", mex.upd_master.board_id);
                        printf("receved data id: %d\n", mex.upd_master.id_data);
                        printf("receved data value: %d\n", mex.upd_master.char_value);
                        printf("-----------------------------------------------\n");
                        break;
                }
            }else {
                printf("receved command with id: %d\n", mex.id.full_id);
                printf("command data[0]: %d\n", mex.data[0]);
                printf("command data[1]: %d\n", mex.data[1]);
                printf("command data[2]: %d\n", mex.data[2]);
                printf("command data[3]: %d\n", mex.data[3]);
                printf("command data[4]: %d\n", mex.data[4]);
                printf("command data[5]: %d\n", mex.data[5]);
                printf("command data[6]: %d\n", mex.data[6]);
                printf("command data[7]: %d\n", mex.data[7]);
                printf("-----------------------------------------------\n");
            }
        }
    };
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 5) {
        printf("invalid input number %d\n\
                argv[1] = board_id\n\
                argv[2] = board_name of size <= 6\n\
                argv[3] = num of vars\n\
                argv[4] = num of coms\n",argc);
        return -1;
    }

    if (strlen(argv[2]) > 6){
        printf("invalid board name %s\n\
                max len: %d\n \
                given: %ld\n",argv[2], BOARD_NAME_SIZE,strlen(argv[2]));
        return -2;
    }

    can_socket = can_init("test_can_dps");
    if (can_socket < 0) {
        printf("error init can\n");
        return -3;
    }

    uint8_t board_id = atoi(argv[1]);
    char* board_name_orig = argv[2];
    uint8_t num_vars = atoi(argv[3]);
    uint8_t num_coms = atoi(argv[4]);

    char board_name[BOARD_NAME_SIZE] = {0};
    memcpy(board_name, board_name_orig, BOARD_NAME_SIZE);
    dps_init(send_f_can, board_id,board_name);

    pthread_t new_thread = 1;
    pthread_create(&new_thread, NULL, check_incomming_message, NULL);

    uint8_t* vars = calloc(num_vars,sizeof(*vars));
    dps_command* coms = calloc(num_coms,sizeof(*coms));

    dps_var new_var = {
        .name = "VXY",
        .size = sizeof(*vars),
    };
    new_var.name[1] = '0' + board_id;
    for (short i=0; i<num_vars; i++) {
        new_var.var_ptr = &vars[i];
        sprintf(&new_var.name[2],"%u" , i); 
        dps_monitor_var(&new_var);
    }

    uint16_t base_id = 15 + board_id;
    dps_command new_com = {
        .name = "CXY",
        // .size = sizeof(*coms),
    };
    new_com.name[1] = '0' + board_id;
    for (short j=0; j<num_coms; j++) {
        sprintf(&new_com.name[2],"%u" , j); 
        new_com.id_can.full_id = base_id + j;
        dps_monitor_command(&new_com);
    }


    dps_print_slave();
    printf("init/conf completed\n");



    // pthread_join(new_thread, NULL);
    free(vars);
    free(coms);
    return 0;
}
