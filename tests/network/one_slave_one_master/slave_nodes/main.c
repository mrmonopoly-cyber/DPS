#include "../../../../dps_slave.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>
#include <pthread.h>

int can_socket = -1;

int send_f_can(CanMessage* mex)
{
    return 0;
}

int main(void)
{
    can_socket = can_init("test_can_dps");
    if (can_socket < 0) {
        printf("error init can\n");
        return -1;
    }

    BoardName board_name = {
        .full_data.name = "SLAVE1",
    };
    dps_init(send_f_can,&board_name);

    return 0;
}
