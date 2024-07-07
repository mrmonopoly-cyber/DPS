#include "../../dps_master.h"
#include <stdio.h>

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

int main(void)
{
    dps_master_init(debug_send);

    dps_master_refresh();

    return 0;
}
