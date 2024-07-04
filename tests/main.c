#include "../dps.h"
#include <stdint.h>

uint8_t send(can_message* mex){
    return 0;
}

int main(void)
{
    dps_init(send);
    return 0;
}
