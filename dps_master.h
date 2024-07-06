#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#include "dps_common.h"
#include "lib/c_vector/c_vector.h"

//INFO: init the master
//send_f : function needed to send a can message
void dps_master_init(can_send send_f);

//INFO: refresh the current set of variable sending a message to all the board 
//in the network
void dps_master_refresh();

//INFO: get a vector with all the BOARDS and their VARIABLES and COMMANDS
//DO NOT FREE THE POINTER
const c_vector* dps_master_board_list();

//INFO: send a mex to a specific board to updated a variable of the board
void dps_master_update(const uint8_t board_id, const uint8_t data_id, const void* value);

//INFO: send a command in the system with a given value
void dps_master_send_command(const can_id id_comm, const void* value);

//INFO: check if a given can message is of the belong to the library
//if it's the case it use it and return 1
//else return 0
uint8_t dps_master_check_can_mex_recv(const can_message* mex);

#endif // !__DPS_MASTER__
