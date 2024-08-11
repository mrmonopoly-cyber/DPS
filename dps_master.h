#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#include "common/can_mex/command.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/obj_id.h"
#include "common/can_mex/board.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"

#include "lib/c_vector/c_vector.h"
#include <stdint.h>

typedef struct{
    char* name;
    uint8_t id;
}board_info;

typedef struct{
    char name[NAME_MAX_SIZE];
    CommandInfoMetadata metadata;
}com_info;

typedef struct{
    char name[NAME_MAX_SIZE];
    VariableInfoMetadata metadata;
}var_record;

typedef struct{
    uint8_t board_num;
    board_info boards[];
}board_list_info;

typedef struct{
    uint8_t board_num;
    var_record vars[];
}var_list_info;

typedef struct{
    uint8_t board_num;
    com_info coms[];
}com_list_info;

//INFO: init the master
//send_f : function needed to send a can message
//return EXIT_SUCCESS if success
//EXIT_FAILURE if errors happens
int dps_master_init(can_send send_f);

//INFO: establish  connection between master and slaves
//return EXIT_SUCCESS if success
//EXIT_FAILURE if errors happens
int dps_master_new_connection();

//INFO: return a list of all the board known by the master with theirs id
board_list_info* dps_master_list_board();

//INFO: return a list of all the vars known by the master in a board
int dps_master_list_vars(uint8_t board_id, var_list_info** o_list);

//INFO: return a list of all the coms known by the master 
int dps_master_list_coms(com_list_info** o_list);

//INFO: check if a message in input is for the library and operate if can
//return EXIT_SUCCESS if the message belongs to the library
//EXIT_FAILURE otherwise
int dps_master_check_mex_recv(CanMessage* mex);

#ifdef __DEBUG__

int dps_master_print_boards();
int dps_master_print_coms();

#endif // __DEBUG__

#endif // !__DPS_MASTER__
