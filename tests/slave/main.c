#include "../../dps_slave.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t send(can_message* mex){
    printf("----------------------------------------\n");
    printf("sending mex: id: %d\n", mex->id.full_id);
    printf("sending mex: mex size: %d\n", mex->mex_size);
    switch (mex->id.full_id) {
        case RESP:
            printf("sending mex: board id: %d\n", mex->info.board_id);
            switch (mex->info.mex_type) {
                case VAR:
                    printf("sending mex: mex type: VAR\n");
                    printf("sending mex: id data: %d\n", mex->info.var_slave.id_data);
                    printf("sending mex: size: %d\n", mex->info.var_slave.data_size);
                    printf("sending mex: name: %s\n", mex->info.var_slave.name);

                    break;
                case COM:
                    printf("sending mex: mex type: COM\n");
                    printf("sending mex: id can: %d\n", mex->info.com_slave.id_can_com);
                    printf("sending mex: nam: %s\n", mex->info.com_slave.name);
                    break;
                case BRD:
                    printf("sending mex: mex type: BRD\n");
                    printf("sending mex: board name: %s\n",mex->info.board_slave.name);
                    break;
            
            }
            break;
    
    }
    return 0;
}

uint8_t a = 0;
uint16_t b = 0;
uint32_t c = 0;
uint8_t d = 0;

int main(void)
{
    uint8_t board_id = 15;
    dps_var new_var = {
        .name = "aa",
        .size = sizeof(a),
        .var_ptr = &a,
    };

    dps_command new_com = {
        .id_can = {5},
        .name = "CM",
    };

    can_message can_mex = {
        .id = {VARS},
        .mex_size = 8,
        .upd_master = {
            .board_id = board_id,
            .id_data = 0,
            .short_value = 3,
        },
    };
    char board_name[6] = "ATC";

    dps_init(send,board_id,board_name);
    dps_monitor_var(&new_var);
    new_var.size = sizeof(b);
    new_var.var_ptr = &b;
    new_var.name[0] = 'b';
    new_var.name[1] = 'b';
    dps_monitor_var(&new_var);
    new_var.var_ptr = &c;
    new_var.name[0] = 'c';
    new_var.name[1] = 'c';
    dps_monitor_var(&new_var);

    dps_monitor_command(&new_com);

    dps_print_var();

    dps_check_can_command_recv(&can_mex);
    memset(can_mex.upd_master.value,0,sizeof(can_mex.upd_master.value));
    can_mex.upd_master.board_id = board_id;
    can_mex.upd_master.id_data = 1;
    can_mex.upd_master.int_value = 4;
    dps_check_can_command_recv(&can_mex);
    memset(can_mex.upd_master.value,0,sizeof(can_mex.upd_master.value));
    can_mex.upd_master.board_id = board_id;
    can_mex.upd_master.id_data = 2;
    can_mex.upd_master.int_value = 5;
    dps_check_can_command_recv(&can_mex);
    memset(can_mex.upd_master.value,0,sizeof(can_mex.upd_master.value));
    can_mex.id.full_id = INFO;
    dps_check_can_command_recv(&can_mex);


    printf("----------------------------------------\n");
    printf("updated var a: %d\n", a);
    printf("updated var b: %d\n", b);
    printf("updated var c: %d\n", c);
    printf("id comm c: %d\n", new_com.id_can.full_id);

    return 0;
}