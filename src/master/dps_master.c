#include "dps_master.h"
#include "../common/common.h"
#include "../../lib/c_vector/c_vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

#define CHECK_INPUT(IN,err)                                                        \
  if (!IN)                                                                     \
    return err;

struct DpsMaster_t{
  can_send send_f;
  c_vector_h* board_vec;
  uint16_t master_id;
  uint16_t slages_id;
  uint8_t obj_ids:4;
};

union DpsMaster_h_t_conv{
  struct DpsMaster_h* const restrict hidden;
  struct DpsMaster_t* const restrict clear;
};

union DpsMaster_h_t_conv_const{
  const struct DpsMaster_h* const restrict hidden;
  const struct DpsMaster_t* const restrict clear;
};

typedef struct {
  uint8_t id;
  char board_name[BOARD_NAME_LENGTH];
  c_vector_h* vars;
}BoardRecordInternal;

typedef struct{
  VarRecord var_specification;
  uint8_t var_id:4;
}VarRecordInternal;

static inline uint8_t new_id(struct DpsMaster_t* const restrict self)
{
  return  self->obj_ids++;;
}

#define CHECK_INIT(self)                                                           \
  if (!self->send_f || !self->board_vec)                                 \
    return EXIT_FAILURE;

static int found_board(const void *list_ele, const void *key) {
  const BoardRecordInternal *board = list_ele;
  const uint8_t *id = key;
  return !(board->id == *id);
}

static int found_var(const void *list_ele, const void *key) {
  const VarRecordInternal *var = list_ele;
  const uint8_t *id = key;
  return !(var->var_id == *id);
}

static void _dummy_func_const(const void *ele __attribute__((__unused__))) {}

static void _dummy_fun(void *ele __attribute__((__unused__))) {}

static void _free_board(void* ele)
{
  BoardRecordInternal* board = ele;
  printf("free board %p\n",ele);
  c_vector_free(board->vars);
  return;
}

static int8_t _send_refresh_request_checked(const struct DpsMaster_t* const restrict self,
    const BoardRecordInternal* const restrict board, const uint8_t var_id)
{
  can_obj_dps_mesages_h_t o = {
    .can_0x28b_DpsMasterMex.Mode = 2,
  };
  struct DpsCanMessage mex;
  if (board)
  {
    const VarRecordInternal *var = c_vector_find(board->vars, &var_id);
    if (var)
    {
      o.can_0x28b_DpsMasterMex.var_metadata_board_id= board->id;
      o.can_0x28b_DpsMasterMex.var_metadata_var_id = var->var_id;
      mex.id = self->master_id;
      mex.dlc = pack_message(&o, CAN_ID_DPSSLAVEMEX, &mex.full_word);

      return self->send_f(&mex);
    }
  }
  return -1;
}

static int8_t
_get_board_name(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_master)
{
  struct c_vector_input_init args =
  {
    .capacity = 10,
    .comp_fun = found_var,
    .ele_size = sizeof(VarRecordInternal),
    .free_fun = _dummy_fun,
    .print_fun = _dummy_func_const,
  };
  BoardRecordInternal new_board =
  {
    .id = new_id(self),
    .vars = c_vector_init(&args),
  };
  memcpy(new_board.board_name, &mex_master->board_name, BOARD_NAME_LENGTH);
  return !!c_vector_push(&self->board_vec, &new_board);
}

static int8_t
_get_var_name(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->board_id);
  if (board)
  {
    VarRecordInternal* var = c_vector_find(board->vars, &mex_slave->info_var_id);
    if (!var)
    {
      VarRecordInternal new_var = {
        .var_id = mex_slave->info_var_id,
      };
      memcpy(new_var.var_specification.name, &mex_slave->var_name, sizeof(VAR_NAME_LENGTH));
      if(!c_vector_push(board->vars, &new_var))
      {
        return -2;
      }
      return 0;
    }
    memcpy(var->var_specification.name, &mex_slave->var_name, sizeof(VAR_NAME_LENGTH));
    return 0;
  }
  return -1;
}

static int8_t
_get_var_metadata(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->value_var_id);
  if (board)
  {
    VarRecordInternal* var = c_vector_find(board->vars, &mex_slave->value_var_id);
    if (!var)
    {
      VarRecordInternal new_var = {
        .var_id = mex_slave->value_var_id,
        .var_specification.size = mex_slave->size,
        .var_specification.type = mex_slave->type,
      };
      if(!c_vector_push(board->vars, &new_var))
      {
        return -2;
      }
      return 0;
    }
    var->var_specification.type = mex_slave->type;
    var->var_specification.size = mex_slave->size;
    return 0;
  }
  return -1;
}

static int8_t
_get_var_value(struct DpsMaster_t* const restrict self,
    const can_0x28a_DpsSlaveMex_t* const restrict mex_slave)
{
  BoardRecordInternal* board = c_vector_find(self->board_vec, &mex_slave->board_id);
  if (board)
  {
    VarRecordInternal* var = c_vector_find(board->vars, &mex_slave->var_id);
    if (!var)
    {
      VarRecordInternal new_var = {
        .var_id = mex_slave->var_id,
      };
      memcpy(&new_var.var_specification.value, &mex_slave->value, sizeof(mex_slave->value));
      if(!c_vector_push(board->vars, &new_var))
      {
        return -2;
      }
      return 0;
    }
    memcpy(&var->var_specification.value, &mex_slave->value, var->var_specification.size);
    return 0;
  }
  return -1;
}

#ifdef DEBUG
char __assert_size_dps_master[(sizeof(DpsMaster_h) == sizeof(struct DpsMaster_t))?1:-1];
#endif /* ifdef DEBUG */

// public
int8_t
dps_master_init(DpsMaster_h* const restrict self,
    const uint16_t master_id,
    const uint16_t slaves_id,
    const can_send send_f)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  if (p_self->board_vec || p_self->send_f) {
    return EXIT_FAILURE;
  }
  CHECK_INPUT(send_f,-1);

  p_self->send_f = send_f;
  p_self->master_id = master_id;
  p_self->slages_id = slaves_id;

  struct c_vector_input_init args = {
    .capacity = 10,
    .ele_size = sizeof(BoardRecordInternal),
    .free_fun = _free_board,
    .print_fun = _dummy_func_const,
    .comp_fun = found_board,
  };
  p_self->board_vec = c_vector_init(&args);

  if (!p_self->board_vec)
  {
    return -1;
  }

  return 0;
}


int8_t
dps_master_new_connection(DpsMaster_h* const restrict self)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_mesages_h_t o;
  struct DpsCanMessage mex;

#ifdef DEBUG
  CHECK_INIT(p_self);
#endif

  o.can_0x28b_DpsMasterMex.Mode =0;
  mex.id = p_self->master_id;
  mex.dlc = pack_message(&o, CAN_ID_DPSMASTERMEX, &mex.full_word);

  return p_self->send_f(&mex);
}

// INFO: send a request info to a specific board fetching variables and commands
int8_t dps_master_request_info_board(DpsMaster_h* const restrict self,
    const uint8_t board_id, const uint8_t data)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_mesages_h_t o = 
  {
    .can_0x28b_DpsMasterMex.Mode =1,
  };
  struct DpsCanMessage mex;

  if (data & REQ_VAR) {
    BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
    if (board) {
      c_vector_clear(board->vars);
      o.can_0x28b_DpsMasterMex.var_metadata_board_id= board_id;

      mex.id = p_self->master_id;
      mex.dlc = pack_message(&o, CAN_ID_DPSMASTERMEX, &mex.full_word);
      p_self->send_f(&mex);
    }
  }

  return 0;
}

const BoardListInfo*
dps_master_list_board(const DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
#ifdef DEBUG
  CHECK_INPUT(self,NULL);
#endif /* ifdef DEBUG */

  const uint8_t len = c_vector_length(p_self->board_vec);

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
int8_t dps_master_list_vars(DpsMaster_h* const restrict self,
    const uint8_t board_id,VarListInfo **o_list)
{
#ifdef DEBUG
  CHECK_INPUT(self,-1);
  CHECK_INPUT(o_list,-2);
#endif /* ifdef DEBUG */
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self);

  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
  VarListInfo *list = NULL;
  if (board)
  {
    uint8_t len = c_vector_length(board->vars);
    *o_list = calloc(len, sizeof(*list) + (len * sizeof(*((*list).vars))));
    list = *o_list;

    for (uint8_t i = 0; i < len; i++) {
      VarRecordInternal *var = c_vector_get_at_index(board->vars, i);
      if (var) {
        list->vars[i] = var->var_specification;
      }
    }
    (*o_list)->var_num = len;

    return 0;
  }

  return -1;
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
  CHECK_INIT(p_self);

  const BoardRecordInternal* board = c_vector_find(p_self->board_vec, &board_id);
  
  return _send_refresh_request_checked(p_self, board, var_id);
}

// INFO: fetch the current value of all variables in a board in the system
int8_t dps_master_refresh_value_var_all(DpsMaster_h* const restrict self,
    const uint8_t board_id)
{
#ifdef DEBUG
  CHECK_INPUT(self,-1);
#endif /* ifdef DEBUG */
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self);
  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);

  if (board) {
    for (uint8_t i = 0; i < c_vector_length(board->vars); i++)
    {
      return _send_refresh_request_checked(p_self, board, i);
    }
  }
  return -1;
}

// INFO: fetch the current value of a variable in a board in the system and put
// in in o_buffer
int8_t dps_master_get_value_var(const DpsMaster_h* const restrict self,
    const uint8_t board_id,
    const uint8_t var_i,
    VarRecord *const restrict o_var)
{
#ifdef DEBUG
  CHECK_INPUT(self,-1);
#endif /* ifdef DEBUG */
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self);

  const BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);

  if (board)
  {
    VarRecordInternal *var = c_vector_find(board->vars, &var_i);
    if (var)
    {
      *o_var = var->var_specification;
      return 0;
    }
  }
  return -1;
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
#ifdef DEBUG
  // CHECK_INPUT(value,-1);
  // CHECK_INPUT(value_size,-1);
#endif /* ifdef DEBUG */

  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_mesages_h_t o = {
    .can_0x28b_DpsMasterMex.Mode = 3,
  };
  struct DpsCanMessage mex;
  CHECK_INIT(p_self);

  BoardRecordInternal *board = c_vector_find(p_self->board_vec, &board_id);
  if (board)
  {
    VarRecordInternal *var = c_vector_find(board->vars, &var_id);
    if (var && value_size <= var->var_specification.size)
    {
      o.can_0x28b_DpsMasterMex.var_value_board_id = board->id;
      o.can_0x28b_DpsMasterMex.var_value_var_id = var->var_id;
      memcpy(&o.can_0x28b_DpsMasterMex.value, value, var->var_specification.size);
      mex.id = p_self->master_id;
      mex.dlc = pack_message(&o,  CAN_ID_DPSMASTERMEX, &mex.full_word);
      return p_self->send_f(&mex);
    }
  }
  return -1;
}

int8_t dps_master_check_mex_recv(DpsMaster_h* const restrict self,
    const struct DpsCanMessage* const restrict mex)
{
#ifdef DEBUG
  CHECK_INPUT(self,-1);
  CHECK_INPUT(mex,-1);
#endif /* ifdef DEBUG */

  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  can_obj_dps_mesages_h_t o;
  CHECK_INIT(p_self);

  unpack_message(&o, CAN_ID_DPSSLAVEMEX, mex->dlc, mex->full_word,0);

  if (mex->id == p_self->slages_id)
  {
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
        return -1;
    }
  }

  return EXIT_FAILURE;
}

int8_t dps_master_destroy(DpsMaster_h* const restrict self)
{
  union DpsMaster_h_t_conv conv = {self};
  struct DpsMaster_t* const restrict p_self = conv.clear;
  c_vector_free(p_self->board_vec);
  return 0;
}


#ifdef DEBUG
int8_t
dps_master_print_boards(DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self);
  
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

int8_t
dps_master_print_vars(DpsMaster_h* const restrict self)
{
  const union DpsMaster_h_t_conv_const conv = {self};
  const struct DpsMaster_t* const restrict p_self = conv.clear;
  CHECK_INIT(p_self);
  for (uint8_t i = 0; i < c_vector_length(p_self->board_vec); i++) {
    BoardRecordInternal *board = c_vector_get_at_index(p_self->board_vec, i);
    if (board) {
      uint8_t vars = c_vector_length(board->vars);
      for (uint8_t j = 0; j < vars; j++) {
        VarRecordInternal *var = c_vector_get_at_index(board->vars, j);
        if (var) {
          printf("board id: %d,", board->id);
          printf("var board: %s\n", board->board_name);
          printf("var id: %d,", var->var_id);
          printf("var name: %s,", var->var_specification.name);
          printf("var size: %d bit,", var->var_specification.size);
          printf("var type: ");
          switch (var->var_specification.size)
          {
            case DATA_SIGNED:
              printf("DATA_SIGNED\n");
              break;
            case DATA_UNSIGNED:
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
  }
  return 0;
}
#else
int8_t dps_master_print_boards(DpsMaster_h* const restrict self){while(1);}
int8_t dps_master_print_vars(DpsMaster_h* const restrict self){while(1);}
#endif /* ifdef DEBUG */
