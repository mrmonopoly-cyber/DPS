#include "../../../../dps_master.h"
#include "../../can_lib/canlib.h"
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
    switch (mex->id.full_id) {
        case VARS:
            printf("sending update var\n");
            printf("sending board id: %d\n",mex->upd_master.board_id);
            printf("sending data id: %d\n",mex->upd_master.id_data);
            printf("sending data value: %d\n",mex->upd_master.char_value);
            break;
        default:
            printf("sending command\n");
            printf("sending board id: %d\n",mex->id.full_id);
            printf("sending command data[0]: %d\n",f.data[0]);
            printf("sending command data[1]: %d\n",f.data[1]);
            printf("sending command data[2]: %d\n",f.data[2]);
            printf("sending command data[3]: %d\n",f.data[3]);
            printf("sending command data[4]: %d\n",f.data[4]);
            printf("sending command data[5]: %d\n",f.data[5]);
            printf("sending command data[6]: %d\n",f.data[6]);
            printf("sending command data[7]: %d\n",f.data[7]);
            break;
    }
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
            switch (mex.id.full_id) {
                case RESP:
                    printf("receved info board id: %d\n", mex.info.board_id);
                    switch (mex.info.mex_type) {
                        case BRD:
                            printf("receved info board mex\n");
                            printf("receved info board name: %s\n", mex.info.board_slave.name);
                            break;
                        case VAR:
                            printf("receved info var mex\n");
                            printf("receved info var data id: %d\n", mex.info.var_slave.id_data);
                            printf("receved info var data size: %d\n", mex.info.var_slave.data_size);
                            printf("receved info var data name: %s\n", mex.info.var_slave.name);
                            break;
                        case COM:
                            printf("receved info com mex\n");
                            printf("receved info com data id: %d\n", mex.info.com_slave.id_can_com);
                            printf("receved info com data name: %s\n", mex.info.com_slave.name);
                            break;
                    }
                    printf("-----------------------------------------------\n");
            }
            if(dps_master_check_can_mex_recv(&mex)){
                fprintf(stderr,"dps master receive a message\n");
            }
        }
        fprintf(stderr,"receive a message\n");
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
    
    fprintf(stderr,"requiring info from slave\n");
    dps_master_refresh();

    char buff[10] = {0};
    while(strcmp(buff, "HELLO")){
        memset(buff, 0, sizeof(buff));
        scanf("%s",buff);
    }

    fprintf(stderr,"printing info board\n ");
    dps_master_print_board();
    fprintf(stderr,"done printing\n ");

    uint8_t new_value = 20;
    dps_master_update(1, 0, &new_value);

    can_id com = {52};
    uint8_t com_value[CAN_MAX_DATA_SIZE] = {16};
    dps_master_send_command(com, com_value);

    pthread_join(new_thread, NULL);
    return 0;
}
