#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include "dps_common.h"


typedef struct dps_var{
    void* var_ptr;
    uint8_t size;
    char name[5];
}dps_var;

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
void dps_init(can_send send_f, uint8_t board_id, char board_name[BOARD_NAME_SIZE]);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var);

//INFO: tell to dps a dps_command the board can receive 
void dps_monitor_command(dps_command* comm);

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t dps_check_can_command_recv(can_message* mex);

void dps_print_var();
#endif // !__DPS_SLAVE__
