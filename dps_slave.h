#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/object.h"
#include "common/messages.h"

typedef void (*post_update_f) (const void* new_var_value);

typedef struct {
  char name[NAME_MAX_SIZE];
  void *var_ptr;
  post_update_f post_update_f;
} VariableInfoPrimitiveType;

typedef struct {
  char name[NAME_MAX_SIZE];
  uint8_t size;
  uint8_t signd_var : 1;
  uint8_t float_var : 1;
  void *var_ptr;
  post_update_f post_update_f;
} VariableInfoGericType;

typedef struct {
  char name[COMMAND_NAME_SIZE];
  CommandInfoMetadata metadata;
} CommandInfo;

// INFO: create dps manager
// send_f : function to send through CAN the data to the external controller
int dps_init(can_send send_f, BoardName *board_name);

int dps_slave_start(void);

int dps_monitor_var_uint8_t(VariableInfoPrimitiveType *var_info);
int dps_monitor_var_uint16_t(VariableInfoPrimitiveType *var_info);
int dps_monitor_var_uint32_t(VariableInfoPrimitiveType *var_info);

int dps_monitor_var_int8_t(VariableInfoPrimitiveType *var_info);
int dps_monitor_var_int16_t(VariableInfoPrimitiveType *var_info);
int dps_monitor_var_int32_t(VariableInfoPrimitiveType *var_info);

int dps_monitor_var_float_t(VariableInfoPrimitiveType *var_info);

// INFO: tell to dps to monitor a variable, if  the name given is too long it
// will truncate it adding a terminator
int dps_monitor_var_general(VariableInfoGericType *var_info);

// INFO: tell to dps a dps_command the board can receive
int dps_monitor_command(CommandInfo *comm_name);

// INFO: check if a can message is for the dps and if it's the case it executes
// the message
int dps_check_can_command_recv(CanMessage *mex);

int dps_slave_disable(void);

#ifdef DEBUG
int dps_get_id();
int dps_print_var();
int dps_print_com();

#endif // DEBUG

#ifdef __cplusplus
}
#endif

#endif // !__DPS_SLAVE__
