#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"

#define VAR_MAX_NAME_SIZE_SLAVE 5

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
void dps_init(can_send send_f, BoardName);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(VariableInfoName* var_name, VariableInfoMetadata* var_metadata);

//INFO: tell to dps a dps_command the board can receive 
void dps_monitor_command(CommandInfoName* comm_name, CommandInfoMetadata* comm_metadata);

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t dps_check_can_command_recv(CanMessage* mex);

void dps_print_slave();
#endif // !__DPS_SLAVE__
