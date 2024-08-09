#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include <stdint.h>

#include "common/can_mex/board.h"
#include "common/messages.h"

typedef struct{
    char name[CAN_MAX_SIZE_MEX - sizeof(ObjectId)];
    void* var_ptr;
}VariableInfoPrimitiveType;

typedef struct{
    char name[CAN_MAX_SIZE_MEX - sizeof(ObjectId)];
    uint8_t size;
    uint8_t signd_var : 1;
    uint8_t float_var : 1;
    void* var_ptr;
}VariableInfoGericType;

typedef struct{
    char name[CAN_MAX_SIZE_MEX - sizeof(ObjectId)];
    uint8_t id;
    uint8_t dlc;
    uint8_t min;
    uint8_t max;
    uint8_t signd_num : 1;
    uint8_t float_num : 1;
}CommandInfo;

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
int dps_init(can_send send_f, BoardName* board_name);

int dps_monitor_var_uint8_t(VariableInfoPrimitiveType* var_info);
int dps_monitor_var_uint16_t(VariableInfoPrimitiveType* var_info);
int dps_monitor_var_uint32_t(VariableInfoPrimitiveType* var_info);

int dps_monitor_var_int8_t(VariableInfoPrimitiveType* var_info);
int dps_monitor_var_int16_t(VariableInfoPrimitiveType* var_info);
int dps_monitor_var_int32_t(VariableInfoPrimitiveType* var_info);

int dps_monitor_var_float_t(VariableInfoPrimitiveType* var_info);

//INFO: tell to dps to monitor a variable
int dps_monitor_var_general(VariableInfoGericType* var_info);

//INFO: tell to dps a dps_command the board can receive 
int dps_monitor_command(CommandInfo* comm_name);

//INFO: check if a can message is for the dps and if it's the case it executes the message
int dps_check_can_command_recv(CanMessage* mex);

#ifdef DEBUG
int dps_get_id();
void dps_print_var();

#endif // DEBUG
#endif // !__DPS_SLAVE__
