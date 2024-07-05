#include "../dps_slave.h"
#include <stdio.h>
#include <stdint.h>

uint8_t send(can_message* mex){
    printf("sending mex: id: %d\t data:%s\n",
            mex->id.full_id,mex->data );
    return 0;
}

uint8_t a = 0;
uint8_t b = 0;
uint8_t c = 0;
uint8_t d = 0;

int main(void)
{
    dps_var new_var = {
        .name = "a",
        .size = sizeof(a),
        .var_ptr = &a,
    };

    can_message can_mex = {
        .id = {0x131},
        .mex_size = 8,
        .data[0] = 0,
        .data[1] = 0,
        .data[2] = 3,
    };

    dps_init(send,0);
    dps_monitor_var(&new_var);
    new_var.var_ptr = &b;
    new_var.name[0] = 'b';
    dps_monitor_var(&new_var);
    new_var.var_ptr = &c;
    new_var.name[0] = 'c';
    dps_monitor_var(&new_var);

    dps_print_var();

    dps_check_can_command_recv(&can_mex);
    can_mex.data[1] = 1,
    can_mex.data[2] = 4,
    dps_check_can_command_recv(&can_mex);
    can_mex.data[1] = 2,
    can_mex.data[2] = 5,
    dps_check_can_command_recv(&can_mex);


    printf("updated var a: %d\n", a);
    printf("updated var b: %d\n", b);
    printf("updated var c: %d\n", c);

    return 0;
}
