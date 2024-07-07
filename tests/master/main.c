#include "../../dps_master.h"

uint8_t debug_send(can_message* mex){
    return 0;
}

int main(void)
{

    dps_master_init(debug_send);

    return 0;
}
