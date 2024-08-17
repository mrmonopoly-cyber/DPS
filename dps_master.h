#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/obj_id.h"
#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/object.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"

#include "lib/c_vector/c_vector.h"
#include <stdint.h>

typedef struct {
  char *name;
  uint8_t id;
} board_info;

typedef struct {
  char name[NAME_MAX_SIZE];
  CommandInfoMetadata metadata;
} com_info;

typedef struct {
  char name[NAME_MAX_SIZE];
  VariableInfoMetadata metadata;
  uint8_t updated : 1;
  char value[NAME_MAX_SIZE];
} var_record;

typedef struct {
  uint8_t board_num;
  board_info boards[];
} board_list_info;

typedef struct {
  uint8_t board_num;
  var_record vars[];
} var_list_info;

typedef struct {
  uint8_t board_num;
  com_info coms[];
} com_list_info;

// INFO: init the master
// send_f : function needed to send a can message
// return EXIT_SUCCESS if success
// EXIT_FAILURE if errors happens
int dps_master_init(can_send send_f);

// INFO: establish  connection between master and slaves
// return EXIT_SUCCESS if success
// EXIT_FAILURE if errors happens
int dps_master_new_connection();

enum REQUEST_INFO { REQ_VAR = (1 << 0), REQ_COM = (1 << 1) };
// INFO: send a request info to a specific board fetching data based on argument
int dps_master_request_info_board(uint8_t board_id, enum REQUEST_INFO data);

// INFO: return a list of all the board known by the master with theirs id
board_list_info *dps_master_list_board();

// INFO: return a list of all the vars known by the master in a board
int dps_master_list_vars(uint8_t board_id, var_list_info **o_list);

// INFO: return a list of all the coms known by the master
int dps_master_list_coms(com_list_info **o_list);

// INFO: fetch the current value of a variable in a board in the system
int dps_master_refresh_value_var(uint8_t board_id, uint8_t var_id);

// INFO: fetch the current value of all variables in a board in the system
int dps_master_refresh_value_var_all(uint8_t board_id);

// INFO: put the current value of a variable in a board in the system and put in
// in o_buffer
int dps_master_get_value_var(uint8_t board_id, uint8_t var_i,
                             var_record *o_var);

// INFO: send and update request for a variable of a board
// if the size is do not fit can message or is greater than the size of the
// variable the message will not be sent and return EXIT_FAILURE
int dps_master_update_var(uint8_t board_id, uint8_t var_id, void *value,
                          uint8_t value_size);

// INFO: send a command with a payload
// if value size is > then the bound of the command metadata or greater to CAN
// payload buffer message is not sent. If value is NULL or value_size is 0
// message is not sent
int dps_master_send_command(uint16_t com_dps_id, void *value,
                            uint8_t value_size);

// INFO: check if a message in input is for the library and operate if can
// return EXIT_SUCCESS if the message belongs to the library
// EXIT_FAILURE otherwise
int dps_master_check_mex_recv(CanMessage *mex);

#ifdef DEBUG

int dps_master_print_boards();
int dps_master_print_coms();
int dps_master_print_vars();

#endif // __DEBUG__

#endif // !__DPS_MASTER__
