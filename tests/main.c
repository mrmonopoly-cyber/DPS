#include "../dps.h"
#include <stdint.h>

uint8_t send(uint8_t id_mex, uint8_t* buffer){
    return 0;
}

int main(void)
{
    dps* self = dps_init(send);
    return 0;
}
