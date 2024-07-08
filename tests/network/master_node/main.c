#include "../../../dps_master.h"
#include "../can_lib/canlib.h"
#include <stdio.h>
#include <string.h>

int can_socket = -1;

uint8_t send_f_can(can_message* mex){
    struct can_frame f = {
        .can_dlc = mex->mex_size,
        .can_id = mex->id.full_id,
    };
    memcpy(f.data, mex->data,mex->mex_size);
    if(can_send_frame(can_socket, &f)){
        return 1;
    }
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

    uint8_t val[CAN_MAX_DATA_SIZE] = {0};
    can_id c = {15};
    struct can_frame f = {
        .can_id = c.full_id,
        .can_dlc = 1,
        .data[0] = 15,
    };
    can_send_frame(can_socket, &f);
    // dps_master_send_command(c, val);

    return 0;
}
