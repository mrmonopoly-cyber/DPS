#include "../../dps_master.h"

#include "../test_lib.h"

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

int debug_send(CanMessage* mex){
    return 0;
}

int run_test(){
    if(dps_master_init(debug_send)){
        FAILED("failed init master");
        return -1;
    }
    PASSED("ok init master");


    if(dps_master_new_connection()){
        FAILED("failed sending new connection request");
        return -1;
    }
    PASSED("ok sending new connection request");

    return 0;
}

int main(void)
{
    run_test();
    print_SCORE();

    return 0;
}
