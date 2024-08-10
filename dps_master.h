#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/obj_id.h"
#include "common/messages.h"

#include "lib/c_vector/c_vector.h"

//INFO: init the master
//send_f : function needed to send a can message
//return EXIT_SUCCESS if success
//EXIT_FAILURE if errors happens
int dps_master_init(can_send send_f);

//INFO: establish  connection between master and slaves
//return EXIT_SUCCESS if success
//EXIT_FAILURE if errors happens
int dps_master_new_connection();

//INFO: check if a message in input is for the library and operate if can
//return EXIT_SUCCESS if the message belongs to the library
//EXIT_FAILURE otherwise
int dps_master_check_mex_recv(CanMessage* mex);

#ifdef __DEBUG__

int dps_master_print_boards();

#endif // __DEBUG__

#endif // !__DPS_MASTER__
