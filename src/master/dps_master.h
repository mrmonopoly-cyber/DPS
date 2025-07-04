#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../common/common.h"

#if UINTPTR_MAX == 0xFFFFFFFF
#define DPS_MASTER_SIZE 16
#define DPS_MASTER_ALIGN 4
#else
#define DPS_MASTER_SIZE 24
#define DPS_MASTER_ALIGN 8
#endif

typedef struct __attribute__((aligned(DPS_MASTER_ALIGN))){
    const uint8_t private_data[DPS_MASTER_SIZE];
}DpsMaster_h;

typedef struct{
  const char* name;
  uint8_t id;
}BoardInfo;

typedef struct{
  char name[VAR_NAME_LENGTH];
  union{
    uint32_t v_u32;
    float v_float;
  };
  uint8_t size;
  enum DATA_GENERIC_TYPE type:2;
}VarRecord;

typedef struct{
  uint8_t board_num;
  BoardInfo boards[];
}BoardListInfo;

typedef struct{
  uint8_t var_num;
  VarRecord vars[];
}VarListInfo;

enum REQUEST_INFO
{
  REQ_VAR = (1 << 0)
};

// INFO: init the master
// send_f : function needed to send a can message
// return EXIT_SUCCESS if success
// EXIT_FAILURE if errors happens
int8_t
dps_master_init(DpsMaster_h* const restrict self,
    const uint16_t master_id,
    const uint16_t slaves_id,
    const can_send send_f);

// INFO: establish connection between master and slaves
// return EXIT_SUCCESS if success
// EXIT_FAILURE if errors happens
int8_t
dps_master_new_connection(DpsMaster_h* const restrict self);

// INFO: send a request info to a specific board fetching data based on argument
int8_t
dps_master_request_info_board(DpsMaster_h* const restrict self,
    const uint8_t board_id, const uint8_t data);

// INFO: return a list of all the board known by the master with theirs id
BoardListInfo*
dps_master_list_board(const DpsMaster_h* const restrict self);

// INFO: return a list of all the vars known by the master in a board
VarListInfo*
dps_master_list_vars(DpsMaster_h* const restrict self, const uint8_t board_id);

// INFO: fetch the current value of a variable in a board in the system
int8_t
dps_master_refresh_value_var(DpsMaster_h* const restrict self,
    const uint8_t board_id, const uint8_t var_id);

// INFO: fetch the current value of all variables in a board in the system
int8_t
dps_master_refresh_value_var_all(DpsMaster_h* const restrict self,
    const uint8_t board_id);

// INFO: put the current value of a variable in a board in the system and put in
// in o_buffer
int8_t
dps_master_get_value_var(const DpsMaster_h* const restrict self,
    const uint8_t board_id,
    const uint8_t var_i,
    VarRecord *const restrict o_var);

// INFO: send and update request for a variable of a board
// if the size is do not fit can message or is greater than the ize of the
// variable the message will not be sent and return EXIT_FAILURE
int8_t
dps_master_update_var(DpsMaster_h* const restrict self,
    const uint8_t board_id,
    const uint8_t var_id,
    const void* const restrict value,
    const uint8_t value_size);

// INFO: check if a message in input is for the library and operate if can
// return EXIT_SUCCESS if the message belongs to the library
// EXIT_FAILURE otherwise
int8_t
dps_master_check_mex_recv(DpsMaster_h* const restrict self,
    const DpsCanMessage* const restrict mex);

int8_t
dps_master_destroy(DpsMaster_h* const restrict self);

#ifdef DEBUG
int8_t dps_master_print_boards(DpsMaster_h* const restrict self);
int8_t dps_master_print_vars(DpsMaster_h* const restrict self);
#endif // __DEBUG__
       
#ifdef __cplusplus
}
#endif

#endif // !__DPS_MASTER__
