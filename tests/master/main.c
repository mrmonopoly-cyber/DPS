#include "../../dps_master.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define num_boards 6
uint8_t board_ids[num_boards] = {0,1,2,3,4,5};
char board_name[][num_boards]={
    "ATC",
    "SMU",
    "MCU",
    "PCU",
    "IMU",
    "STE",
};

uint8_t debug_send(can_message* mex){
    switch (mex->id.full_id) {
        case INFO:
            printf("sending INFO message\n");
            break;
        case VARS:
            printf("sending VARS message\n");
            printf("sending mex: board id: %d\n",mex->upd_master.board_id);
            printf("sending mex: data id: %d\n",mex->upd_master.id_data);
            printf("sending mex: data char version: %d\n",mex->upd_master.char_value);
            printf("sending mex: data short version: %d\n",mex->upd_master.short_value);
            printf("sending mex: data int version: %d\n",mex->upd_master.int_value);
            break;
        default:
            printf("sending command message with id: %d\n", mex->id.full_id);
            for (int i =0; i<CAN_MAX_DATA_SIZE; i++) {
                printf("data buffer position %d with value : %d\n",i, mex->data[i]);
            }

    }
    return 0;
}

void save_var(char name_var[4], uint8_t data_size, uint8_t id_data,uint8_t id_board){
    can_message recv_mex;

    recv_mex.id.full_id = RESP;
    recv_mex.info.board_id = id_board;
    recv_mex.info.mex_type = VAR;
    recv_mex.info.var_slave.id_data = id_data;
    recv_mex.info.var_slave.data_size = data_size;
    memcpy(recv_mex.info.var_slave.name, name_var, sizeof(recv_mex.info.var_slave.name));
    dps_master_check_can_mex_recv(&recv_mex);
}

void save_com(char name_com[4], can_id* id_com){
    can_message recv_mex;

    recv_mex.id.full_id = RESP;
    recv_mex.info.mex_type = COM;
    recv_mex.info.com_slave.id_can_com = id_com->full_id;
    memcpy(recv_mex.info.com_slave.name, name_com, sizeof(recv_mex.info.com_slave.name));
    dps_master_check_can_mex_recv(&recv_mex);
}

int main(void)
{
    can_message recv_mex;

    dps_master_init(debug_send);
    dps_master_refresh();


    //SIMULATE RECV DATA FROM SLAVES
    recv_mex.id.full_id = RESP;
    //SEDING BOARD INF (id,name)
    recv_mex.info.mex_type = BRD;
    for (int i=0; i<num_boards; i++) {
        recv_mex.info.board_id = board_ids[i];
        memcpy(recv_mex.info.board_slave.name, board_name[i], sizeof(board_name[i]));
        dps_master_check_can_mex_recv(&recv_mex);
    }

    char wrong_board[6] = "ERROR";
    recv_mex.info.board_id = board_ids[0];
    memcpy(recv_mex.info.board_slave.name, wrong_board, sizeof(wrong_board));
    dps_master_check_can_mex_recv(&recv_mex);


    //SEDING BOARD VARS (board id, data id, name, data_size)
    char name_var_zero_zero[4] = "aaaa";
    save_var(name_var_zero_zero,sizeof(uint8_t),0,board_ids[0]);

    char name_var_zero_one[4] = "bb";
    save_var(name_var_zero_one,sizeof(uint8_t),1,board_ids[0]);

    char name_com_zero[4] = "CAL";
    can_id id_com = {87};
    save_com(name_com_zero, &id_com);

    char err_name_com_zero[4] = "EC";
    save_com(err_name_com_zero, &id_com);

    uint8_t new_value = 8;
    dps_master_update(board_ids[0], 0, &new_value);

    uint8_t com_value[8] = {1,2,3,4,5,6,7,12};
    dps_master_send_command(id_com, com_value);

    dps_master_print_board();

    return 0;
}
