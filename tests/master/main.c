#include "../../dps_master.h"
#include <stdint.h>

#define num_boards 6
uint8_t board_ids[num_boards] = {0,1,2,3,4,5};
char board_name[][num_boards]={
    "ATC",
    "SMU",
    "MCU",
    "PCU",
    "IMU",
    "STE",
};

uint8_t debug_send(CanMessage* mex){
    return 0;
}

int main(void)
{
    return 0;
}
