#include "../../../../dps_master.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>

#include <stdlib.h>
#include <threads.h>

int can_socket = -1;

int check_input_mex(void* args)
{
    while (1) {
        struct can_frame frame;
        CanMessage dps_mex;
        if(can_recv_frame(can_socket, &frame)){
            return EXIT_FAILURE;
        }

        dps_mex.id = frame.can_id;
        dps_mex.dlc = frame.can_dlc;
        memcpy(dps_mex.rawMex.raw_buffer, frame.data, frame.can_dlc);
        dps_master_check_mex_recv(&dps_mex);
    }
}

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

    thrd_t th;
    thrd_create(&th, check_input_mex, NULL);

    thrd_join(th, NULL);
    return 0;
}
