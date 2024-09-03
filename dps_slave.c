#include "dps_slave.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/mex_types.h"
#include "common/can_mex/base_mex_components/obj_id.h"
#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/info_req.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"
#include "lib/c_vector/c_vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

#define CHECK_INIT()                                                           \
  if (!dps.vars || !dps.coms) {                                                \
    return EXIT_FAILURE;                                                       \
  }
#define CHECK_INPUT(input)                                                     \
  if (!input) {                                                                \
    return EXIT_FAILURE;                                                       \
  }

struct slave_dps {
  char board_name[BOARD_NAME_LENGTH];
  can_send send_f;
  c_vector *vars;
  c_vector *coms;
  int8_t board_id;
};

struct var_internal {
  uint8_t var_id : DATA_ID_SIZE_BIT;
  VariableInfoGericType data;
};

static struct slave_dps dps;
static uint16_t object_id_slave = 0;

static void dummy_fun(void *e) {}

static int found_var(const void *list_ele, const void *key) {
  const struct var_internal *ele_l = list_ele;
  const uint8_t *id = key;

  return ele_l->var_id == *id;
}

static int found_com(const void *list_ele, const void *key) {
  const CommandInfo *ele_l = list_ele;
  const uint16_t *id = key;

  return ele_l->metadata.full_data.com_id == *id;
}

static void print_var(const void *ele) {
  const struct var_internal *var = ele;
  printf("var_id :%d,", var->var_id);
  printf("var_name :%s,", var->data.name);
  printf("var_ptr :%d,", *(char *)var->data.var_ptr);
  printf("var_size :%d,", var->data.size);
  printf("var_signed :%d,", var->data.signd_var);
  printf("var_float :%d\n", var->data.float_var);
}

static void print_com(const void *ele) {
  const CommandInfo *com = ele;
  printf("com_id :%d,", com->metadata.full_data.com_id);
  printf("com_name :%s\n", com->name);
  printf("com_dlc :%d\n", com->metadata.full_data.size);
  printf("com signed :%d\n", com->metadata.full_data.signe_num);
  printf("com floated :%d\n", com->metadata.full_data.float_num);
  printf("com board:%d\n", com->metadata.full_data.board_id);
}

static uint8_t new_id() {
  object_id_slave++;
  return object_id_slave - 1;
}

static int req_inf_exec(CanMessage *mex) {
  ReqInfo data_mex = {
      .raw_data = mex->dps_payload.data,
  };
  VariableInfoName var_name;
  VariableInfoMetadata var_metadata;
  VariableValue var_value;
  CanMessage new_mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
  };

  switch (data_mex.full_data.info_t) {
  case VAR:
    for (uint8_t i = 0; i < c_vector_length(dps.vars); i++) {
      struct var_internal *var = c_vector_get_at_index(dps.vars, i);

      // var name
      var_name.full_data.obj_id.board_id = dps.board_id;
      var_name.full_data.obj_id.data_id = var->var_id;
      memcpy(var_name.full_data.name, var->data.name, sizeof(var->data.name));
      new_mex.dps_payload.mext_type.type = VAR_NAME;
      new_mex.dps_payload.data = var_name.raw_data;
      dps.send_f(&new_mex);

      // var  metadata
      var_metadata.full_data.obj_id.board_id = dps.board_id;
      var_metadata.full_data.obj_id.data_id = var->var_id;
      var_metadata.full_data.size = var->data.size;
      var_metadata.full_data.float_num = var->data.float_var;
      var_metadata.full_data.signe_num = var->data.signd_var;
      new_mex.dps_payload.mext_type.type = VAR_METADATA;
      new_mex.dps_payload.data = var_metadata.raw_data;
      if (dps.send_f(&new_mex)) {
        return EXIT_FAILURE;
      }
    }
    return EXIT_SUCCESS;
    break;
  case COMMAND:
    for (uint8_t i = 0; i < c_vector_length(dps.coms); i++) {
      CommandInfo *com = c_vector_get_at_index(dps.coms, i);

      // name
      CommandInfoName com_name = {
          .full_data.com_id = com->metadata.full_data.com_id,
      };
      memcpy(com_name.full_data.name, com->name, sizeof(com->name));
      new_mex.dps_payload.mext_type.type = COM_NAME;
      new_mex.dps_payload.data = com_name.raw_data;
      if(dps.send_f(&new_mex)){
          return EXIT_FAILURE;
      }

      // metadata
      com->metadata.full_data.board_id = dps.board_id;
      new_mex.dps_payload.mext_type.type = COM_METADATA;
      new_mex.dps_payload.data = com->metadata.raw_data;
      if (dps.send_f(&new_mex)) {
        return EXIT_FAILURE;
      }
    }
    return EXIT_SUCCESS;
    break;
  case VAR_VALUE:
    if (data_mex.full_data.data_it.board_id == dps.board_id) {
      uint8_t var_id = data_mex.full_data.data_it.data_id;
      struct var_internal *var = c_vector_find(dps.vars, &var_id);
      if (!var) {
        return EXIT_FAILURE;
      }
      var_value.full_data.obj_id.board_id = dps.board_id;
      var_value.full_data.obj_id.data_id = var->var_id;
      memcpy(var_value.full_data.value, var->data.var_ptr, var->data.size);
      new_mex.dps_payload.data = var_value.raw_data;
      new_mex.dps_payload.mext_type.type = GET_CURRENT_VAR_VALUE;
      return dps.send_f(&new_mex);
    }
    return EXIT_FAILURE;
    break;
  default:
    return EXIT_FAILURE;
    break;
  }
  return EXIT_FAILURE;
}

static int set_board_id_exec(CanMessage *mex) {
  AssignBoarId board_payload = {
      .raw_data = mex->dps_payload.data,
  };
  if (!strncmp(dps.board_name, board_payload.full_data.name,
               BOARD_NAME_LENGTH)) {
    dps.board_id = board_payload.full_data.obj_id.board_id;
  }
  return EXIT_SUCCESS;
}

static int set_var_value_exec(CanMessage *mex) {
  VariableModify new_value = {
      .raw_data = mex->dps_payload.data,
  };

  if (new_value.full_data.obj_id.board_id == dps.board_id) {
    uint8_t var_id = new_value.full_data.obj_id.data_id;
    struct var_internal *saved_var = c_vector_find(dps.vars, &var_id);
    if (saved_var) {
      memcpy(saved_var->data.var_ptr, new_value.full_data.value,
              saved_var->data.size);
    }
  }

  return EXIT_SUCCESS;
}

static int new_connection_exec() {
  BoardName resp_payload;
  memcpy(resp_payload.full_data.name, dps.board_name, BOARD_NAME_LENGTH);
  CanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload.data = resp_payload.raw_data,
  };
  if (dps.send_f(&mex)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// public
int dps_init(can_send send_f, BoardName *board_name) {
    if (dps.vars || dps.coms || dps.send_f) {
        return EXIT_FAILURE;
    }

  CHECK_INPUT(send_f);
  CHECK_INPUT(board_name);
  CHECK_INPUT(board_name->full_data.name[0])

  memcpy(dps.board_name, board_name->full_data.name, BOARD_NAME_LENGTH);
  dps.send_f = send_f;
  dps.board_id = -1;
  {
    struct c_vector_input_init vars = {
        .capacity = 2,
        .ele_size = sizeof(struct var_internal),
        .free_fun = dummy_fun,
        .print_fun = print_var,
        .found_f = found_var,
    };

    struct c_vector_input_init coms = {
        .capacity = 10,
        .ele_size = sizeof(CommandInfo),
        .free_fun = dummy_fun,
        .print_fun = print_com,
        .found_f = found_com,
    };
    dps.vars = c_vector_init(&vars);
    if (!dps.vars) {
      return EXIT_FAILURE;
    }
    dps.coms = c_vector_init(&coms);
    if (!dps.coms) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int dps_monitor_var_uint8_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(uint8_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 0,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int dps_monitor_var_uint16_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(uint16_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 0,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int dps_monitor_var_uint32_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(uint32_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 0,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int dps_monitor_var_int8_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(uint8_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 1,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int dps_monitor_var_int16_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(int16_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 1,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int dps_monitor_var_int32_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(int32_t),
              .var_ptr = var_info->var_ptr,
              .float_var = 0,
              .signd_var = 1,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int dps_monitor_var_float_t(VariableInfoPrimitiveType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data =
          {
              .size = sizeof(float),
              .var_ptr = var_info->var_ptr,
              .float_var = 1,
              .signd_var = 1,
          },
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// INFO: tell to dps to monitor a variable
int dps_monitor_var_general(VariableInfoGericType *var_info) {
  CHECK_INIT();

  CHECK_INPUT(var_info);
  CHECK_INPUT(var_info->var_ptr);
  CHECK_INPUT(var_info->name);

  struct var_internal new_var = {
      .var_id = new_id(),
      .data = *var_info,
  };
  memcpy(new_var.data.name, var_info->name, sizeof(new_var.data.name) - 1);

  if (!c_vector_push(&dps.vars, &new_var)) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/*
 * INFO: tell to dps a dps_command the board can receive
 * board_id field can be ignored, will be overwritten in any case by the library
 */
int dps_monitor_command(CommandInfo *com) {
  CHECK_INIT();

  CHECK_INPUT(com);
  CHECK_INPUT(com->name);
  CHECK_INPUT(com->metadata.full_data.com_id);
  CHECK_INPUT(com->metadata.full_data.size);

  com->metadata.full_data.board_id = dps.board_id;
  if ((*com).metadata.full_data.size > CAN_PROTOCOL_MAX_PAYLOAD_SIZE) {
    return EXIT_FAILURE;
  }

  if (!c_vector_push(&dps.coms, com)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// INFO: check if a can message is for the dps and if it's the case it executes
// the message
int dps_check_can_command_recv(CanMessage *mex) {
  CHECK_INIT();

  CHECK_INPUT(mex);

  if (mex->id == DPS_CAN_MESSAGE_ID) {
    switch (mex->dps_payload.mext_type.type) {
    case GET_INFO:
      return req_inf_exec(mex);
    case SET_BOARD_ID:
      return set_board_id_exec(mex);
    case SET_CURRENT_VAR_VALUE:
      return set_var_value_exec(mex);
    case NEW_CONNECTION:
      return new_connection_exec();
      break;
    }
  }

  return EXIT_FAILURE;
}

// debug
int dps_get_id() {
  CHECK_INIT();

  return dps.board_id;
}

int dps_print_var() {
  CHECK_INIT();

  struct var_internal *var = NULL;
  uint8_t len = c_vector_length(dps.vars);
  for (uint8_t i = 0; i < len; i++) {
    var = c_vector_get_at_index(dps.vars, i);
    print_var(var);
  }
  return EXIT_SUCCESS;
}

int dps_print_com() {
  CHECK_INIT();

  struct com_internal *com = NULL;
  uint8_t len = c_vector_length(dps.coms);
  for (uint8_t i = 0; i < len; i++) {
    com = c_vector_get_at_index(dps.coms, i);
    print_com(com);
  }
  return EXIT_SUCCESS;
}
