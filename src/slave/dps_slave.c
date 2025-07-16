#include "dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

#define MAX_NUM_VARS 16

struct VarInternal
{
  void* p_var;
  post_update post_update_fun;
  char var_name[VAR_NAME_LENGTH];
  uint8_t size;
  enum DATA_GENERIC_TYPE type: 2;
};

struct DpsSlave_t{
  char board_name[BOARD_NAME_LENGTH];
  can_send send_f;
  wait_after_send wait_f;
  struct VarInternal vars[MAX_NUM_VARS];
  uint8_t vars_len;
  uint8_t var_bit_map[MAX_NUM_VARS / 8];
  int8_t board_id;
  uint16_t master_id;
  uint16_t slave_id;
  uint8_t enable : 1;
};


union DpsSlave_h_t_conv
{
  DpsSlave_h* const hidden;
  struct DpsSlave_t* const clear;
};

union DpsSlave_h_t_conv_const
{
  const DpsSlave_h* const hidden;
  const struct DpsSlave_t* const clear;
};

#define CHECK_INIT(self)if (!self->vars) return -1;
#define CHECK_INPUT(input) if (!input) return -1;

#ifdef DEBUG
char __assert_size_dps_slave[(sizeof(DpsSlave_h) == sizeof(struct DpsSlave_t))?1:-1];
char __assert_align_dps_slave[(_Alignof(DpsSlave_h) == _Alignof(struct DpsSlave_t))?1:-1];
#endif /* ifdef DEBUG */

#ifdef DEBUG
static void _print_var(const void *ele, const uint8_t var_pos)
{
  const struct VarInternal *var = ele;
  printf("var_id :%d,",var_pos);
  printf("var_name :%s,", var->var_name);
  printf("var_ptr :%p,", var->p_var);
  printf("var_size :%d,", var->size);
  printf("var type: ");
  switch (var->type)
  {
    case DATA_UNSIGNED:
      printf("DATA_UNSIGNED\n");
      break;
    case DATA_SIGNED:
      printf("DATA_SIGNED\n");
      break;
    case DATA_FLOATED:
      printf("DATA_FLOATED\n");
      break;
    default:
      break;
  }
}
#endif /* ifdef DEBUG */

static inline int8_t _send_mex_and_wait(const struct DpsSlave_t* const restrict self,
    const DpsCanMessage* const restrict mex)
{
  int8_t err=0;
  err=self->send_f(mex);
  if (self->wait_f)
  {
    self->wait_f();
  }
  return err;
}

static inline int8_t _push_new_var(struct DpsSlave_t* const restrict self,
    const struct VarInternal* new_var)
{
  uint8_t *bit_map = self->var_bit_map;
  uint32_t cursor = 0;
  uint32_t quozient =0;
  uint32_t rest=0;
  while (bit_map[quozient] & (1u << rest) && cursor < MAX_NUM_VARS )
  {
    cursor++;
    quozient = cursor/8;
    rest = cursor%8;
  }

  if (cursor >= MAX_NUM_VARS)
  {
    return -2;
  }

  if ((1u<<rest) ^ bit_map[quozient])
  {
    self->vars[cursor] = *new_var;
    self->vars_len++;
    bit_map[quozient] |= (1u<<rest);
  }
  else
  {
    return -3;
  }

  return (int8_t) cursor;
}

static int8_t _discover_board(const struct DpsSlave_t* const restrict self)
{
 DpsCanMessage mex;
 can_obj_dps_messages_h_t o = 
  {
    .can_0x28a_DpsSlaveMex.Mode = 0,
    .can_0x28a_DpsSlaveMex.board_id = self->board_id,
  };
  memcpy(&o.can_0x28a_DpsSlaveMex.board_name, self->board_name, BOARD_NAME_LENGTH);
  mex.id = self->slave_id;
  mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSSLAVEMEX, &mex.full_word);

  return _send_mex_and_wait(self, &mex);
}

static int8_t _request_infos(struct DpsSlave_t* const restrict self,
    const can_0x28b_DpsMasterMex_t* const restrict req_mex)
{
  can_obj_dps_messages_h_t o = 
  {
    .can_0x28a_DpsSlaveMex.board_id = self->board_id,
  };
  DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));

  if (req_mex->var_name_board_id!= self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<sizeof(self->vars)/sizeof(self->vars[0]); i++)
  {
    struct VarInternal* var = &self->vars[i];
    if (var->p_var)
    {
      //send name infos
      o.can_0x28a_DpsSlaveMex.Mode = 1;
      o.can_0x28a_DpsSlaveMex.info_var_id = i;
      memcpy(&o.can_0x28a_DpsSlaveMex.var_name, var->var_name, VAR_NAME_LENGTH);
      mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSSLAVEMEX, &mex.full_word);
      mex.id = self->slave_id;
      (void)_send_mex_and_wait(self, &mex);

      //send metadata infos
      o.can_0x28a_DpsSlaveMex.Mode = 2;
      o.can_0x28a_DpsSlaveMex.value_var_id= i;
      o.can_0x28a_DpsSlaveMex.type = var->type;
      o.can_0x28a_DpsSlaveMex.size = var->size;
      memcpy(&o.can_0x28a_DpsSlaveMex.var_name, var->var_name, VAR_NAME_LENGTH);
      mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSSLAVEMEX, &mex.full_word);
      mex.id = self->slave_id;
      (void)_send_mex_and_wait(self, &mex);
    }
  }

  return 0;
}


static int8_t _request_var_value(const struct DpsSlave_t* const restrict self,
    const can_0x28b_DpsMasterMex_t* const restrict req_value_mex)
{
  can_obj_dps_messages_h_t o = 
  {
    .can_0x28a_DpsSlaveMex.Mode = 3,
    .can_0x28a_DpsSlaveMex.board_id = self->board_id,
    .can_0x28a_DpsSlaveMex.value = 0,
  };
  DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));
  if (req_value_mex->var_refresh_board_id!= self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<self->vars_len; i++)
  {
    const struct VarInternal* var = &self->vars[i];
    if (var)
    {
      switch (var->size)
      {
        case 0:
          o.can_0x28a_DpsSlaveMex.value = *(uint8_t*) var->p_var;
          break;
        case 1:
          o.can_0x28a_DpsSlaveMex.value = *(uint16_t*) var->p_var;
          break;
        case 2:
          o.can_0x28a_DpsSlaveMex.value = *(uint32_t*) var->p_var;
          break;
      }
      o.can_0x28a_DpsSlaveMex.var_id= i;
      mex.dlc = (uint8_t) pack_message(&o, CAN_ID_DPSSLAVEMEX, &mex.full_word);
      mex.id = self->slave_id;
      (void) _send_mex_and_wait(self, &mex);
    }
  }

  return 0;
}

static int8_t _update_var_value(const struct DpsSlave_t* const restrict self,
    const can_0x28b_DpsMasterMex_t* const restrict update_value_mex)
{
  DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));
  if (update_value_mex->var_value_board_id != self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<self->vars_len; i++)
  {
    const struct VarInternal* var = &self->vars[i];
    if (var && i == update_value_mex->var_value_var_id)
    {
      uint8_t size =0;
      switch (var->size)
      {
        case 0:
          size = 1;
          break;
        case 1:
          size = 2;
          break;
        case 2:
          size = 4;
          break;
        default:
          return -2;
      }
      memcpy(var->p_var, &update_value_mex->value, size);
      if (var->post_update_fun)
      {
        if(var->post_update_fun(var->var_name, var->p_var)<0)
        {
          return -4;
        }
      }
      return 0;
    }
  }

  return -3;
}

// public
int8_t dps_slave_init(DpsSlave_h* const restrict self,
        can_send send_f,
        const wait_after_send wait_f,
        const char board_name[BOARD_NAME_LENGTH],
        const uint8_t dps_board_id,
        const uint16_t dps_can_id_master,
        const uint16_t dps_can_id_slaves)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
  CHECK_INPUT(self);
  if (p_self->send_f || p_self->enable)
  {
    return -1;
  }

  if (dps_can_id_slaves == dps_can_id_master)
  {
    return -2;
  }

  memset(self, 0, sizeof(*self));

  memcpy(p_self->board_name, board_name, BOARD_NAME_LENGTH);
  p_self->send_f = send_f;
  p_self->wait_f = wait_f;
  p_self->board_id = dps_board_id;
  p_self->slave_id= dps_can_id_slaves;
  p_self->master_id = dps_can_id_master;
  p_self->enable = 0;

  return 0;
}

int8_t dps_slave_start(DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
  if (p_self->enable)
  {
    return -1;
  }

  p_self->enable= 1;

  return 0;
}

int8_t dps_slave_disable(DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INPUT(self);
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */
  if (!p_self->enable)
  {
    return -1;
  }

  p_self->enable = 0;

  return 0;
}

int8_t dps_monitor_primitive_var(DpsSlave_h* const restrict self,
        const enum DPS_PRIMITIVE_TYPES type,
        void* const p_data,
        post_update post_update_f,
        const char name[VAR_NAME_LENGTH])
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
#ifdef DEBUG
  CHECK_INPUT(self);
  CHECK_INPUT(name);
  CHECK_INPUT(p_data);

  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */
  if (!p_self->enable)
  {
    return -1;
  }


  struct VarInternal new_var = {
      .p_var = p_data,
      .post_update_fun = post_update_f,
  };
  memcpy(new_var.var_name, name, VAR_NAME_LENGTH);
  switch (type)
  {
    case DPS_TYPES_UINT8_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 0;
      break;
    case DPS_TYPES_UINT16_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 1;
      break;
    case DPS_TYPES_UINT32_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 2;
      break;
    case DPS_TYPES_INT8_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 0;
      break;
    case DPS_TYPES_INT16_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 1;
      break;
    case DPS_TYPES_INT32_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 2;
      break;
    case DPS_TYPES_FLOAT_T:
      new_var.type = DATA_FLOATED;
      new_var.size = 2;
      break;
    default:
      return -1;
  }


  return _push_new_var(p_self, &new_var);
}

// INFO: check if a can message is for the dps and if it's the case it executes
// the message
int8_t dps_slave_check_can_command_recv(DpsSlave_h* const restrict self,
        const DpsCanMessage* const mex)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
  can_obj_dps_messages_h_t o;

  CHECK_INPUT(self);
  CHECK_INIT(p_self);

  if (!p_self->enable)
  {
    return -1;
  }
  if (mex->id == p_self->master_id)
  {
    if(unpack_message(&o, CAN_ID_DPSMASTERMEX, mex->full_word, mex->dlc, 0)<0)
    {
      return -1;
    }
    switch (o.can_0x28b_DpsMasterMex.Mode)
    {
      case 0: //discover board
        return _discover_board(p_self);
      case 1: //request infos
        return _request_infos(p_self, &o.can_0x28b_DpsMasterMex);
      case 2: //request var value
        return _request_var_value(p_self, &o.can_0x28b_DpsMasterMex);
      case 3: //update var value
        return _update_var_value(p_self, &o.can_0x28b_DpsMasterMex);
      default:
        return -2;
      break;
    }
  }

  return -1;
}

void dps_slave_destroy(DpsSlave_h* const restrict self)
{
  memset(self, 0, sizeof(*self));
}

#ifdef DEBUG
int dps_get_id(const DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv_const conv = {self};
  const struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */

  if (!p_self->enable)
  {
    return -1;
  }

  return p_self->board_id;
}

int dps_print_var(const DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv_const conv = {self};
  const struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */

  if (!p_self->enable)
  {
    return -1;
  }

  const struct VarInternal *var = NULL;
  uint8_t len = p_self->vars_len;
  for (uint8_t i = 0; i < len; i++) {
    var = &p_self->vars[i];
    _print_var(var,i);
  }
  return 0;
}
#else
int dps_get_id(const DpsSlave_h* const restrict self){while (1);}
int dps_print_var(const DpsSlave_h* const restrict self){while(1);}
#endif /* ifdef DEBUG */
