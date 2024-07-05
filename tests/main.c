#include "../dps_slave.h"
#include <stdio.h>
#include <stdint.h>

uint8_t send(can_message* mex){
    dps_command* c;

    printf("----------------------------------------\n");
    printf("sending mex: id: %d\n", mex->id.full_id);
    printf("sending mex: mex size: %d\n", mex->mex_size);
    printf("sending mex: board id: %d\n", mex->data[0]);
    printf("sending mex: data type: %d\n", mex->data[1]);
    switch (mex->data[1]) {
        case VAR:
            printf("sending mex: id data: %d\n", mex->data[2]);
            printf("sending mex: name: %s\n", &mex->data[3]);
            break;
        case COM:
            c = (dps_command *) &mex->data[2];
            printf("sending mex: id can: %d\n", c->id_can.full_id);
            printf("sending mex: nam: %s\n", c->name);
            break;
        case BRD:
            printf("sending mex: board name: %s\n",mex->board_slave.name);
            break;
    }
    return 0;
}

uint8_t a = 0;
uint8_t b = 0;
uint8_t c = 0;
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
        .data[0] = board_id,
        .data[1] = 0,
        .data[2] = 3,
    };
    char board_name[6] = "ATC";

    dps_init(send,board_id,board_name);
    dps_monitor_var(&new_var);
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
    can_mex.upd_master.board_id = board_id;
    can_mex.upd_master.id_data = 1;
    can_mex.data[2] = 4,
    dps_check_can_command_recv(&can_mex);
    can_mex.data[1] = 2,
    can_mex.data[2] = 5,
    dps_check_can_command_recv(&can_mex);
    can_mex.id.full_id = INFO;
    dps_check_can_command_recv(&can_mex);


    printf("----------------------------------------\n");
    printf("updated var a: %d\n", a);
    printf("updated var b: %d\n", b);
    printf("updated var c: %d\n", c);
    printf("id comm c: %d\n", new_com.id_can.full_id);

    return 0;
}
