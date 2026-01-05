#include "dps_master.h"
#include "../common/common.h"
#include "../../lib/c_vector/c_vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

#define VAR_ID_SIZE_BIT 4

#define CHECK_INPUT(IN,err)                                                        \
  if (!IN)                                                                     \
    return err;

struct DpsMaster_t{
  DpsCommon common;
  c_vector_h* board_vec;
  uint16_t master_id;
  uint16_t slaves_id;
  uint8_t obj_ids:4;
};

union DpsMaster_h_t_conv{
  DpsMaster_h* const hidden;
  struct DpsMaster_t* const clear;
};

union DpsMaster_h_t_conv_const{
  const DpsMaster_h* const hidden;
  const struct DpsMaster_t* const clear;
};

  VarRecord var_specification;

typedef struct {
  uint8_t id;
  char board_name[BOARD_NAME_LENGTH];
  VarRecord vars[2 << VAR_ID_SIZE_BIT];
  uint8_t vars_length;
}BoardRecordInternal;


#ifdef DEBUG
char __assert_size_dps_master[(sizeof(DpsMaster_h) == sizeof(struct DpsMaster_t))?1:-1];
char __assert_align_dps_master[(__alignof(DpsMaster_h) == __alignof(struct DpsMaster_t))?1:-1];
#endif /* ifdef DEBUG */

//private

#define _send_mex_and_wait(self, mex) send_mex_and_wait(&self->common, mex)

#define CHECK_INIT(self, err)                                                           \
  if (!self->common.send_f || !self->board_vec)                                 \
    return err;

static int _found_board(const void *list_ele, const void *key) {
  const BoardRecordInternal *board = list_ele;
  const uint8_t *id = key;
  return !(board->id == *id);
}

static VarRecord* _get_var(BoardRecordInternal* board, const uint8_t var_id)
{
  if(var_id>=board->vars_length)
  {
    board->vars_length++;
  }
  return &board->vars[var_id];
}

static int8_t _send_refresh_request_checked(const struct DpsMaster_t* const restrict self,
    const BoardRecordInternal* const restrict board, const uint8_t var_id)
{
  can_obj_dps_messages_h_t o = {
    .can_0x28b_DpsMasterMex.Mode = 2,
    .can_0x28b_DpsMasterMex.var_refresh_board_id= board->id,
  };
  DpsCanMessage mex={0};
  if (board)
  {
    const VarRecord *var = &board->vars[var_id];
    if (var)
    {
      o.can_0x28b_DpsMasterMex.var_refresh_var_id= var_id;
      mex.id = self->master_id;
      mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSMASTERMEX, &mex.full_word);

      return _send_mex_and_wait(self, &mex);
    }
  }
  return -1;
}

static int8_t _get_board_name(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_master)
{
  BoardRecordInternal new_board =
  {
    .id = mex_master->board_id,
  };
  memcpy(new_board.board_name, &mex_master->board_name, BOARD_NAME_LENGTH);
  return !!c_vector_push(&self->board_vec, &new_board);
}

static int8_t _get_var_name(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->board_id);
  if (board)
  {
    VarRecord* var = _get_var(board, mex_slave->info_var_id);
    memcpy(var->name, &mex_slave->var_name, VAR_NAME_LENGTH);
    return 0;
  }
  return -1;
}

static int8_t _get_var_metadata(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->board_id);
  if (board)
  {
    VarRecord* var = _get_var(board, mex_slave->value_var_id);
    var->type = mex_slave->type;
    var->size = mex_slave->size;
    return 0;
  }
  return -1;
}

static int8_t _get_var_value(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->board_id);
  const void* const restrict p_data = &mex_slave->value;

  if (board)
  {
    VarRecord* var = _get_var(board, mex_slave->var_id);
    if (var)
    {
      switch (var->size)
      {
        case 0:
        case 1:
        case 2:
          memcpy(&var->v_u32, p_data, sizeof(var->v_u32));
          break;
        case 3:

          switch (mex_slave->half)
          {
            case 0: //low
              memcpy(&var->incomplete_value[0], p_data, sizeof(uint32_t));
              var->modified_low_half =1;
              break;
            case 1: //high
              memcpy(&var->incomplete_value[1], p_data, sizeof(uint32_t));
              var->modified_high_half =1;
              break;
          }
          if (var->modified_low_half && var->modified_high_half)
          {
            memcpy(&var->v_u64, var->incomplete_value, sizeof(var->v_u64));
          }
          break;
      }
    }
    return 0;
  }
  return -1;
}

// public
int8_t dps_master_init(DpsMaster_h* const restrict self,
    const uint16_t master_id,
    const uint16_t slaves_id,
    const can_send send_f,
    const wait_after_send wait_f)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  if (p_self->board_vec || p_self->common.send_f) {
    return EXIT_FAILURE;
  }
  CHECK_INPUT(send_f,-1);

  p_self->common.send_f = send_f;
  p_self->common.wait_f = wait_f;
  p_self->master_id = master_id;
  p_self->slaves_id = slaves_id;

  struct c_vector_input_init args = {
    .capacity = 10,
    .ele_size = sizeof(BoardRecordInternal),
    .free_fun = NULL,
    .print_fun = NULL,
    .comp_fun = _found_board,
  };
  p_self->board_vec = c_vector_init(&args);

  if (!p_self->board_vec)
  {
    return -1;
  }

  return 0;
}


int8_t dps_master_new_connection(DpsMaster_h* const restrict self)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_messages_h_t o={0};
  DpsCanMessage mex={0};

#ifdef DEBUG
  CHECK_INIT(p_self, -1);
#endif

  o.can_0x28b_DpsMasterMex.Mode =0;
  mex.id = p_self->master_id;
  mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSMASTERMEX, &mex.full_word);

  return _send_mex_and_wait(p_self, &mex);
}

// INFO: send a request info to a specific board fetching variables and commands
int8_t dps_master_request_info_board(DpsMaster_h* const restrict self,
    const uint8_t board_id, const uint8_t data)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_messages_h_t o = 
  {
    .can_0x28b_DpsMasterMex.Mode =1,
  };
  DpsCanMessage mex;

  if (data & REQ_VAR) {
    BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
    if (board)
    {
      memset(board->vars, 0, sizeof(board->vars));
      o.can_0x28b_DpsMasterMex.var_name_board_id= board_id;

      mex.id = p_self->master_id;
      mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSMASTERMEX, &mex.full_word);
      _send_mex_and_wait(p_self, &mex);
    }
  }

  return 0;
}

BoardListInfo* dps_master_list_board(const DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
#ifdef DEBUG
  CHECK_INPUT(self,NULL);
#endif /* ifdef DEBUG */

  const uint8_t len = c_vector_length(p_self->board_vec);

  if (!len)
  {
    return NULL;
  }

  BoardListInfo *res = calloc(1, sizeof(BoardListInfo) + (sizeof(BoardInfo) * len));
  res->board_num = len;

  for (uint8_t i = 0; i < len; i++) {
    BoardRecordInternal *board = c_vector_get_at_index(p_self->board_vec, i);
    if (!board) {
      free(res);
      return NULL;
    }
    res->boards[i].id = board->id;
    res->boards[i].name = board->board_name;
  }

  return res;
}

// INFO: return a list of all the vars known by the master in a board
VarListInfo* dps_master_list_vars(DpsMaster_h* const restrict self, const uint8_t board_id)
{
#ifdef DEBUG
  CHECK_INPUT(self,NULL);
#endif /* ifdef DEBUG */
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, NULL);

  BoardRecordInternal* const board = c_vector_find(p_self->board_vec, &board_id);
  VarListInfo *list = NULL;
  if (board && board->vars_length)
  {
    const uint8_t len = board->vars_length;
    list = calloc(1, sizeof(*list) + (len * sizeof(*((*list).vars))));

    for (uint8_t i = 0; i < len; i++) {
      const VarRecord* const var = &board->vars[i];
      if (var) {
        list->vars[i] = *var;
      }
    }
    list->var_num = len;
  }

  return list;
}

// INFO: fetch the current value of a variable in a board in the system
int8_t dps_master_refresh_value_var(DpsMaster_h* const restrict self,
    const uint8_t board_id, const uint8_t var_id)
{
#ifdef DEBUG
  CHECK_INPUT(self,-1);
#endif /* ifdef DEBUG */
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, -1);

  const BoardRecordInternal* board = c_vector_find(p_self->board_vec, &board_id);
  
  return _send_refresh_request_checked(p_self, board, var_id);
}

// INFO: fetch the current value of all variables in a board in the system
int8_t dps_master_refresh_value_var_all(DpsMaster_h* const restrict self,
    const uint8_t board_id)
{
  CHECK_INPUT(self,-1);
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, -2);
  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
  int8_t err=0;

  if (board)
  {
    for (uint8_t i = 0; i < board->vars_length; i++)
    {
      int8_t temp_err = _send_refresh_request_checked(p_self, board, i);
      err = temp_err<0 ? temp_err : err;
    }
  }
  return err;
}

// INFO: fetch the current value of a variable in a board in the system and put
// in in o_buffer
int8_t dps_master_get_value_var(const DpsMaster_h* const restrict self,
    const uint8_t board_id,
    const uint8_t var_i,
    VarRecord *const restrict o_var)
{
  CHECK_INPUT(self,-1);
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, -2);

  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);

  if (board)
  {
    VarRecord *var = _get_var(board, var_i);
    if (var)
    {
      *o_var = *var;
      return 0;
    }
  }
  return -3;
}

// INFO: send and update request for a variable of a board
// if the size is do not fit can message or is greater than the size of the
// variable the message will not be sent and return EXIT_FAILURE
int8_t dps_master_update_var(DpsMaster_h* const restrict self,
    const uint8_t board_id,
    const uint8_t var_id,
    const void* const restrict value,
    const uint8_t value_size)
{
  CHECK_INPUT(value,-1);
  CHECK_INPUT(value_size,-2);

  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;

  DpsCanMessage mex={0};
  can_obj_dps_messages_h_t o ={
    .can_0x28b_DpsMasterMex = {
      .Mode = 3,
      .var_value_board_id = board_id,
      .var_value_var_id = var_id,
    },
  };

  CHECK_INIT(p_self, -3);

  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
  if (board)
  {
    VarRecord *var = _get_var(board, var_id);
    if (var)
    {
      const uint8_t size = (uint8_t) (1u << var->size);
      const uint32_t* restrict const ptr_data = value;
      if (value_size <= size)
      {
        int err =0;


        switch (size)
        {
          case 8:
            memcpy(&o.can_0x28b_DpsMasterMex.value, &ptr_data[0] , sizeof(ptr_data[0]));
            o.can_0x28b_DpsMasterMex.half = 0; //low
            mex.id = p_self->master_id;
            err = pack_message(&o,  CAN_ID_DPSMASTERMEX, &mex.full_word);

            if (err <0 )
            {
              return (int8_t) -(70 + err);
            }

            mex.dlc = (uint8_t) err;
            if(_send_mex_and_wait(p_self, &mex)<0)
            {
              return -99;
            }

            memcpy(&o.can_0x28b_DpsMasterMex.value, &ptr_data[1] , sizeof(ptr_data[0]));
            o.can_0x28b_DpsMasterMex.half = 1; //high
            mex.id = p_self->master_id;
            err = pack_message(&o,  CAN_ID_DPSMASTERMEX, &mex.full_word);

            if (err <0 )
            {
              return (int8_t) -(70 + err);
            }

            mex.dlc = (uint8_t) err;
            return _send_mex_and_wait(p_self, &mex);

            break;
          case 1:
          case 2:
          case 4:
            o.can_0x28b_DpsMasterMex.var_value_var_id = var_id;
            o.can_0x28b_DpsMasterMex.var_value_board_id = board_id;
            o.can_0x28b_DpsMasterMex.Mode = 3;
            o.can_0x28b_DpsMasterMex.half = 0; //low
            memcpy(&o.can_0x28b_DpsMasterMex.value, &ptr_data[0], size);
            mex.id = p_self->master_id;
            err = pack_message(&o,  CAN_ID_DPSMASTERMEX, &mex.full_word);

            if (err <0 )
            {
              return (int8_t) -(70 + err);
            }

            mex.dlc = (uint8_t) err;
            return _send_mex_and_wait(p_self, &mex);
            break;
          default:
            return -4;
        }
      }
      return -5;
    }
  }
  return -6;
}

int8_t dps_master_check_mex_recv(DpsMaster_h* const restrict self,
    const DpsCanMessage* const restrict mex)
{
  CHECK_INPUT(self,-1);
  CHECK_INPUT(mex,-2);

  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_messages_h_t o={0};
  CHECK_INIT(p_self, -3);

  if (mex->id == p_self->slaves_id)
  {
    if(unpack_message(&o, CAN_ID_DPSSLAVEMEX, mex->full_word, mex->dlc, 0)<0)
    {
      return -4;
    }
    switch (o.can_0x28a_DpsSlaveMex.Mode)
    {
      case 0:
        return _get_board_name(p_self, &o.can_0x28a_DpsSlaveMex);
      case 1:
        return _get_var_name(p_self, &o.can_0x28a_DpsSlaveMex);
      case 2:
        return _get_var_metadata(p_self, &o.can_0x28a_DpsSlaveMex);
      case 3:
        return _get_var_value(p_self, &o.can_0x28a_DpsSlaveMex);
      default:
        return -5;
    }
  }

  return 0;
}

int8_t dps_master_destroy(DpsMaster_h* const restrict self)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  c_vector_free(p_self->board_vec);
  return 0;
}


#ifdef DEBUG
int8_t dps_master_print_boards(DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, -1);
  
  for (uint8_t i = 0; i < c_vector_length(p_self->board_vec); i++) {
    BoardRecordInternal *board = c_vector_get_at_index(p_self->board_vec, i);
    if (!board)
    {
      printf("board not found at index: %d\n",i);
    }
    else
    {
      printf("board id: %d,", board->id);
      printf("board name: %s\n", board->board_name);
    }
  }

  return EXIT_SUCCESS;
}

int8_t dps_master_print_vars(DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self, -1);

  for (uint8_t i = 0; i < c_vector_length(p_self->board_vec); i++) {
    BoardRecordInternal *board = c_vector_get_at_index(p_self->board_vec, i);
    if (board)
    {
      const uint8_t vars = board->vars_length;
      for (uint8_t j = 0; j < vars; j++) {
        VarRecord *var = &board->vars[i];
        printf("board id: %d, ", board->id);
        printf("board name: %s, ", board->board_name);
        printf("var id: %d,", i);
        printf("var name: %s,", var->name);
        printf("var size: %d, ", 1 << var->size);
        printf("var type: ");
        switch (var->type)
        {
          case DATA_UNSIGNED:
            printf("DATA_UNSIGNED\n");
            break;
          case DATA_SIGNED:
            printf("DATA_UNSIGNED\n");
            break;
          case DATA_FLOATED:
            printf("DATA_FLOATED\n");
            break;
          default:
            printf("INVALID TYPE\n");
            break;
        }
      }
    }
  }
  return 0;
}
#else
int8_t dps_master_print_boards(DpsMaster_h* const restrict self){while(1);}
int8_t dps_master_print_vars(DpsMaster_h* const restrict self){while(1);}
#endif /* ifdef DEBUG */
