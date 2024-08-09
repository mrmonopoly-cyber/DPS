#include "dps_slave.h"
#include <stdlib.h>


int dps_init(can_send send_f, BoardName* board_name)
{
    return EXIT_FAILURE;
}

int dps_monitor_var_uint8_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_uint16_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_uint32_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_uint64_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}

int dps_monitor_var_int8_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_int16_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_int32_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_int64_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}

int dps_monitor_var_float_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}
int dps_monitor_var_double_t(VariableInfoPrimitiveType* var_info)
{
    return EXIT_FAILURE;
}

//INFO: tell to dps to monitor a variable
int dps_monitor_var_general(VariableInfoGericType* var_info)
{
    return EXIT_FAILURE;
}

//INFO: tell to dps a dps_command the board can receive 
int dps_monitor_command(CommandInfo* comm_name)
{
    return EXIT_FAILURE;
}

//INFO: check if a can message is for the dps and if it's the case it executes the message
int dps_check_can_command_recv(CanMessage* mex)
{
    return EXIT_FAILURE;
}

int dps_get_id()
{
    return 0;
}
