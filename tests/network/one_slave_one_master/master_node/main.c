#include "../../../../dps_master.h"
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
    dps_master_init(send_f_can);
    return 0;
}
