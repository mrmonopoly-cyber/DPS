#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include <stdint.h>

#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/object.h"
#include "common/messages.h"

typedef struct{
    char name[NAME_MAX_SIZE];
    void* var_ptr;
}VariableInfoPrimitiveType;

typedef struct{
    char name[NAME_MAX_SIZE];
    uint8_t size;
    uint8_t signd_var : 1;
    uint8_t float_var : 1;
    void* var_ptr;
}VariableInfoGericType;

typedef struct{
    char name[NAME_MAX_SIZE];
    CommandInfoMetadata metadata;
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
int dps_print_var();
int dps_print_com();


#endif // DEBUG
#endif // !__DPS_SLAVE__
