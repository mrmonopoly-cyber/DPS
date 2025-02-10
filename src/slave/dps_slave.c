#include "../../lib/c_vector/c_vector.h"
#include "../common/messages/messages.h"
#include "dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

struct DpsSlave_t{
  char board_name[BOARD_NAME_LENGTH];
  can_send send_f;
  c_vector_h vars;
  int8_t board_id;
  uint8_t obj_ids;
  uint16_t master_id;
  uint16_t slave_id;
  uint8_t enable : 1;
};

struct VarInternal
{
  void* p_var;
  post_update post_update_fun;
  char var_name[VAR_NAME_LENGTH];
  uint8_t size;
  uint8_t var_id : DATA_ID_SIZE_BIT;
  enum DATA_GENERIC_TYPE type: 2;
};

union DpsSlave_h_t_conv
{
  struct DpsSlave_h* hidden;
  struct DpsSlave_t* clear;
};

union DpsSlave_h_t_conv_const
{
  const struct DpsSlave_h* hidden;
  const struct DpsSlave_t* clear;
};

#define CHECK_INIT(self)if (!self->vars) return -1;
#define CHECK_INPUT(input) if (!input) return EXIT_FAILURE;

#ifdef DEBUG
char __assert_size_dps_slave[(sizeof(struct DpsSlave_h) == sizeof(struct DpsSlave_t))?1:-1];

#endif /* ifdef DEBUG */

static void dummy_fun(void* ele)
{
}

static int cmp_fun_var(const void* ele_list, const void* key)
{
  const struct VarInternal* l_var = ele_list;
  const struct VarInternal* k_var = key;
  
  return !(l_var->var_id == k_var->var_id);
}

static void print_var(const void *ele)
{
  const struct VarInternal *var = ele;
  printf("var_id :%d,", var->var_id);
  printf("var_name :%s,", var->var_name);
  printf("var_ptr :%d,", *(char *)var->p_var);
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

static inline uint8_t new_id(struct DpsSlave_t* const restrict self)
{
  return self->obj_ids++;
}

static int8_t discover_board(const struct DpsSlave_t* const restrict self)
{
  struct DpsCanMessage mex;
  struct DpsMessagesObj o = 
  {
    .dps_slave_mex.Mode = 0,
    .dps_slave_mex.board_id = self->board_id,
  };
  memcpy(&o.dps_slave_mex.Mode_0.name, self->board_name, BOARD_NAME_LENGTH);
  mex.dlc = dps_messages_pack(&o, DPS_MESSAGES_SLAVE, &mex.full_word);

  return self->send_f(&mex);
}

static int8_t request_infos(struct DpsSlave_t* const restrict self,
    const struct MasterMode1 * const restrict req_mex)
{
  struct DpsMessagesObj o = 
  {
    .dps_slave_mex.Mode = 1,
    .dps_slave_mex.board_id = self->board_id,
  };
  struct DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));

  if (req_mex->board_id != self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<c_vector_length(self->vars); i++)
  {
    struct VarInternal* var = c_vector_get_at_index(self->vars,i);
    if (var)
    {
      o.dps_slave_mex.Mode_1.var_id = var->var_id;
      memcpy(&o.dps_slave_mex.Mode_1.name, var->var_name, VAR_NAME_LENGTH);
      mex.dlc = dps_messages_pack(&o, DPS_MESSAGES_SLAVE, &mex.full_word);
      self->send_f(&mex);
    }
  }

  return 0;
}

static int8_t update_var_value(const struct DpsSlave_t* const restrict self,
    const struct MasterMode3* const restrict update_value_mex)
{
  struct DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));
  if (update_value_mex->board_id != self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<c_vector_length(self->vars); i++)
  {
    struct VarInternal* var = c_vector_get_at_index(self->vars,i);
    if (var && var->var_id == update_value_mex->var_id)
    {
      memcpy(var->p_var, &update_value_mex->value, var->size);
      if (var->post_update_fun)
      {
        var->post_update_fun(var->p_var);
      }
      return 0;
    }
  }

  return -2;
}

static int8_t request_var_value(const struct DpsSlave_t* const restrict self,
    const struct MasterMode2* const restrict req_value_mex)
{
  struct DpsMessagesObj o = 
  {
    .dps_slave_mex.Mode = 2,
    .dps_slave_mex.board_id = self->board_id,
  };
  struct DpsCanMessage mex;
  memset(&mex, 0, sizeof(mex));
  if (req_value_mex->board_id != self->board_id)
  {
    return -1;
  }

  for (uint8_t i=0; i<c_vector_length(self->vars); i++)
  {
    struct VarInternal* var = c_vector_get_at_index(self->vars,i);
    if (var)
    {
      o.dps_slave_mex.Mode_2.var_id = var->var_id;
      o.dps_slave_mex.Mode_2.size = var->size;
      o.dps_slave_mex.Mode_2.type = var->type;
      mex.dlc = dps_messages_pack(&o, DPS_MESSAGES_SLAVE, &mex.full_word);
      self->send_f(&mex);
    }
  }

  return 0;
}

// public
int8_t
dps_init(DpsSlave_h* const restrict self,
        can_send send_f,
        const char board_name[BOARD_NAME_LENGTH],
        const uint8_t dps_board_id,
        const uint16_t dps_can_id_master,
        const uint16_t dps_can_id_slaves)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
#ifdef DEBUG
  CHECK_INPUT(self);
  if (p_self->vars || p_self->send_f || p_self->enable)
  {
    return EXIT_FAILURE;
  }
#endif /* ifdef DEBUG */

  memcpy(p_self->board_name, board_name, BOARD_NAME_LENGTH);
  p_self->send_f = send_f;
  p_self->board_id = dps_board_id;
  p_self->slave_id= dps_can_id_slaves;
  p_self->master_id = dps_can_id_master;
  struct c_vector_input_init vars =
  {
    .capacity = 2,
    .ele_size = sizeof(struct VarInternal),
    .free_fun = dummy_fun,
    .print_fun = print_var,
    .comp_fun = cmp_fun_var,
  };

  p_self->vars = c_vector_init(&vars);
  if (!p_self->vars)
  {
    return EXIT_FAILURE;
  }

  p_self->enable = 0;
  return EXIT_SUCCESS;
}

int8_t dps_slave_start(DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
  if (p_self->enable)
  {
    return EXIT_FAILURE;
  }

  p_self->enable= 1;

  return EXIT_SUCCESS;
}

int8_t
dps_slave_disable(DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INPUT(self);
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */
  if (!p_self->enable)
  {
    return EXIT_FAILURE;
  }

  p_self->enable = 0;

  return EXIT_SUCCESS;
}

int8_t
dps_monitor_primitive_var(DpsSlave_h* const restrict self,
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
    return EXIT_FAILURE;
  }


  struct VarInternal new_var = {
      .var_id = new_id(p_self),
      .p_var = p_data,
      .post_update_fun = post_update_f,
  };
  switch (type)
  {
    case DPS_TYPES_UINT8_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 1;
      break;
    case DPS_TYPES_UINT16_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 2;
      break;
    case DPS_TYPES_UINT32_T:
      new_var.type = DATA_UNSIGNED;
      new_var.size = 4;
      break;
    case DPS_TYPES_INT8_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 1;
      break;
    case DPS_TYPES_INT16_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 2;
      break;
    case DPS_TYPES_INT32_T:
      new_var.type = DATA_SIGNED;
      new_var.size = 4;
      break;
    case DPS_TYPES_FLOAT_T:
      new_var.type = DATA_FLOATED;
      new_var.size = 4;
      break;
    default:
      return -1;
  }

  memcpy(new_var.var_name, name, VAR_NAME_LENGTH);

  if (!c_vector_push(&p_self->vars, &new_var))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// INFO: check if a can message is for the dps and if it's the case it executes
// the message
int8_t
dps_check_can_command_recv(DpsSlave_h* const restrict self,
        const struct DpsCanMessage* const mex)
{
  union DpsSlave_h_t_conv conv = {self};
  struct DpsSlave_t* const restrict p_self = conv.clear;
  struct DpsMessagesObj o;

#ifdef DEBUG
  CHECK_INPUT(self);
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */

  if (!p_self->enable)
  {
    return EXIT_FAILURE;
  }
  if (mex->id == p_self->master_id)
  {
    dps_messages_unpack(&o, DPS_MESSAGES_MASTER, mex->full_word);
    switch (o.dps_master_mex.Mode)
    {
      case 0: //discover board
        return discover_board(p_self);
      case 1: //request infos
        return request_infos(p_self, &o.dps_master_mex.Mode_1);
      case 2: //request var value
        return request_var_value(p_self, &o.dps_master_mex.Mode_2);
      case 3: //update var value
        return update_var_value(p_self, &o.dps_master_mex.Mode_3);
      default:
        return -2;
      break;
    }
  }

  return -1;
}

#ifdef DEBUG
int
dps_get_id(const DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv_const conv = {self};
  const struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */

  if (!p_self->enable)
  {
    return EXIT_FAILURE;
  }

  return p_self->board_id;
}

int
dps_print_var(const DpsSlave_h* const restrict self)
{
  union DpsSlave_h_t_conv_const conv = {self};
  const struct DpsSlave_t* const restrict p_self = conv.clear;

#ifdef DEBUG
  CHECK_INIT(p_self);
#endif /* ifdef DEBUG */

  if (p_self->enable) {
    return EXIT_FAILURE;
  }

  struct VarInternal *var = NULL;
  uint8_t len = c_vector_length(p_self->vars);
  for (uint8_t i = 0; i < len; i++) {
    var = c_vector_get_at_index(p_self->vars, i);
    print_var(var);
  }
  return EXIT_SUCCESS;
}
#else
int dps_get_id(const DpsSlave_h* const restrict self){while (1);}
int dps_print_var(const DpsSlave_h* const restrict self){while(1);}
#endif /* ifdef DEBUG */
