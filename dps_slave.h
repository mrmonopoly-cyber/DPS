#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"

#define VAR_MAX_NAME_SIZE_SLAVE 5

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
int dps_init(can_send send_f, BoardName* board_name);

int dps_monitor_var_uint8_t(VariableInfoName* var_name, uint8_t* var_ptr);
int dps_monitor_var_uint16_t(VariableInfoName* var_name, uint16_t* var_ptr);
int dps_monitor_var_uint32_t(VariableInfoName* var_name, uint32_t* var_ptr);
int dps_monitor_var_uint64_t(VariableInfoName* var_name, uint64_t* var_ptr);

int dps_monitor_var_int8_t(VariableInfoName* var_name, int8_t* var_ptr);
int dps_monitor_var_int16_t(VariableInfoName* var_name, int16_t* var_ptr);
int dps_monitor_var_int32_t(VariableInfoName* var_name, int32_t* var_ptr);
int dps_monitor_var_int64_t(VariableInfoName* var_name, int64_t* var_ptr);

int dps_monitor_var_float_t(VariableInfoName* var_name, float* var_ptr);
int dps_monitor_var_double_t(VariableInfoName* var_name, double* var_ptr);

//INFO: tell to dps to monitor a variable
int dps_monitor_var_general(VariableInfoName* var_name, VariableInfoMetadata* var_metadata);

//INFO: tell to dps a dps_command the board can receive 
int dps_monitor_command(CommandInfoName* comm_name, CommandInfoMetadata* comm_metadata);

//INFO: check if a can message is for the dps and if it's the case it executes the message
int dps_check_can_command_recv(CanMessage* mex);

int dps_print_slave();
#endif // !__DPS_SLAVE__
