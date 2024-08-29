#include "dps_master.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/mex_types.h"
#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/info_req.h"
#include "common/can_mex/new_connection.h"
#include "common/can_mex/object.h"
#include "common/can_mex/variable.h"
#include "common/messages.h"
#include "lib/c_vector/c_vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

#define CHECK_INPUT(IN)                                                        \
  if (!IN)                                                                     \
    return EXIT_FAILURE;

struct dps_master {
  can_send send_f;
  c_vector *boards;
  c_vector *coms;
};

typedef struct {
  uint8_t id;
  char board_name[BOARD_NAME_LENGTH];
  c_vector *vars;
} board_record;

typedef struct {
  char name[NAME_MAX_SIZE];
  CommandInfoMetadata metadata;
  board_record *board;
} com_record;

static uint8_t new_id() {
  static uint8_t id_generator = 0;
  id_generator++;
  return id_generator - 1;
}

static struct dps_master dps;
#define CHECK_INIT()                                                           \
  if (!dps.send_f || !dps.boards || !dps.coms)                                 \
    return EXIT_FAILURE;

static int found_board(const void *ele, const void *key) {
  const board_record *board = ele;
  const uint8_t *id = key;

  return board->id == *id;
}

static int found_com(const void *ele, const void *key) {
  const com_record *com = ele;
  const uint8_t *id = key;

  return com->metadata.full_data.com_id == *id;
}

static int found_var(const void *ele, const void *key) {
  const var_record *var = ele;
  const uint8_t *id = key;

  return var->metadata.full_data.obj_id.data_id == *id;
}

static void print_com(const void *ele) {
  const com_record *com = ele;
  printf("com name: %s,", com->name);
  printf("com id: %d,", com->metadata.full_data.com_id);
  printf("com size: %d,", com->metadata.full_data.size);
  printf("com float: %d,", com->metadata.full_data.float_num);
  printf("com signed num: %d,", com->metadata.full_data.signe_num);
  printf("com board: %s\n", com->board->board_name);
}

static void dummy_func_const(const void *ele) {}

static void dummy_fun(void *ele) {}

static int get_board_name_exec(CanMessage *mex) {
  BoardName b_name = {
      .raw_data = mex->dps_payload.data,
  };

  struct c_vector_input_init args = {
      .capacity = 10,
      .ele_size = sizeof(var_record),
      .print_fun = dummy_func_const,
      .free_fun = dummy_fun,
      .found_f = found_var,
  };

  board_record new_board = {
      .id = new_id(),
      .vars = c_vector_init(&args),
  };
  memcpy(new_board.board_name, b_name.full_data.name, BOARD_NAME_LENGTH);

  if (!c_vector_push(&dps.boards, &new_board)) {
    return EXIT_FAILURE;
  }

  AssignBoarId board_id = {
      .full_data.obj_id.board_id = new_board.id,
  };
  memcpy(board_id.full_data.name, new_board.board_name, BOARD_NAME_LENGTH);

  CanMessage mex_id = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload =
          {
              .mext_type = {SET_BOARD_ID},
              .data = board_id.raw_data,
          },
  };

  return dps.send_f(&mex_id);
}

static int get_var_name_exec(CanMessage *mex) {
  VariableInfoName var_name = {
      .raw_data = mex->dps_payload.data,
  };

  uint8_t board_id = var_name.full_data.obj_id.board_id;
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    var_record new_var = {
        .metadata.full_data.obj_id = var_name.full_data.obj_id,
    };
    memcpy(new_var.name, var_name.full_data.name,
           sizeof(var_name.full_data.name));

    if (!c_vector_push(&board->vars, &new_var)) {
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

static int get_var_metadata_exec(CanMessage *mex) {
  VariableInfoMetadata var_metadata = {
      .raw_data = mex->dps_payload.data,
  };

  uint8_t board_id = var_metadata.full_data.obj_id.board_id;
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    uint8_t var_id = var_metadata.full_data.obj_id.data_id;
    var_record *new_var = c_vector_find(board->vars, &var_id);
    if (new_var) {
      new_var->metadata = var_metadata;
      if(dps_master_refresh_value_var(board_id, var_id)){
          return EXIT_FAILURE;
      }
    }
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

static int get_com_name_exec(CanMessage *mex) {
  CommandInfoName com_name = {
      .raw_data = mex->dps_payload.data,
  };

  com_record *old_com = NULL;
  com_record new_com = {
      .metadata.full_data.com_id = com_name.full_data.com_id,
  };

  old_com = c_vector_find(dps.coms, &new_com.metadata.full_data.com_id);
  if (old_com) {
      memcpy(old_com->name, com_name.full_data.name, NAME_MAX_SIZE);
      return EXIT_FAILURE;
  }

  memcpy(new_com.name, com_name.full_data.name, NAME_MAX_SIZE);

  if (!c_vector_push(&dps.coms, &new_com)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static int get_com_metadata_exec(CanMessage *mex) {
  CommandInfoMetadata com_metadata = {
      .raw_data = mex->dps_payload.data,
  };

  uint8_t b_id = com_metadata.full_data.board_id;
  board_record *board = c_vector_find(dps.boards, &b_id);
  if (!board) {
    return EXIT_FAILURE;
  }

  uint16_t id = com_metadata.full_data.com_id;
  com_record *saved_com = c_vector_find(dps.coms, &id);
  if (!saved_com) {
    return EXIT_FAILURE;
  }
  saved_com->board = board;
  saved_com->metadata = com_metadata;

  return EXIT_SUCCESS;
}

static int get_curr_var_value_exec(CanMessage *mex) {
  VariableValue var_val = {
      .raw_data = mex->dps_payload.data,
  };

  uint8_t board_id = var_val.full_data.obj_id.board_id;
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    uint8_t var_id = var_val.full_data.obj_id.data_id;
    var_record *var = c_vector_find(board->vars, &var_id);
    if (var) {
      memcpy(var->value, var_val.full_data.value, var->metadata.full_data.size);
      var->updated = 1;
      return EXIT_SUCCESS;
    }
  }

  return EXIT_FAILURE;
}

static int send_refresh_req_var(uint8_t board_id, var_record *var) {
  var->updated = 0;
  ReqInfo req_value = {
      .full_data.info_t = VAR_VALUE,
      .full_data.data_it.board_id = board_id,
      .full_data.data_it.data_id = var->metadata.full_data.obj_id.data_id,
  };

  CanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload =
          {
              .mext_type = {GET_INFO},
              .data = req_value.raw_data,
          },
  };
  return dps.send_f(&mex);
}

// public
int dps_master_init(can_send send_f) {
  if (dps.coms && dps.boards && dps.send_f) {
    return EXIT_FAILURE;
  }

  CHECK_INPUT(send_f);

  dps.send_f = send_f;
  {
    struct c_vector_input_init args = {
        .capacity = 10,
        .found_f = found_board,
        .ele_size = sizeof(board_record),
        .free_fun = dummy_fun,
        .print_fun = dummy_func_const,
    };
    dps.boards = c_vector_init(&args);
  }

  if (!dps.boards) {
      goto exit;
  }

  {
    struct c_vector_input_init args = {
        .capacity = 10,
        .found_f = found_com,
        .ele_size = sizeof(com_record),
        .free_fun = dummy_fun,
        .print_fun = print_com,
    };
    dps.coms = c_vector_init(&args);
  }
  if (!dps.coms) {
      goto free_coms;
  }

  return EXIT_SUCCESS;

free_coms:
  c_vector_free(dps.boards);
  dps.boards = NULL;
exit:
  return EXIT_FAILURE;
}

int dps_master_new_connection() {
  CHECK_INIT();

  new_connection conn = {};
  CanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload =
          {
              .mext_type = {NEW_CONNECTION},
              .data = conn.raw_data,
          },
  };

  return dps.send_f(&mex);
}

// INFO: send a request info to a specific board fetching variables and commands
int dps_master_request_info_board(uint8_t board_id, uint8_t data) {
  CanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload =
          {
              .mext_type = {GET_INFO},
          },
  };

  if (data & REQ_VAR) {
    board_record *board = c_vector_find(dps.boards, &board_id);
    if (board) {
        c_vector_clear(board->vars);
        ReqInfo info = {
            .full_data.data_it.board_id = board_id,
            .full_data.info_t = VAR,
        };
        mex.dps_payload.data = info.raw_data;
        if (dps.send_f(&mex)) {
            return EXIT_FAILURE;
        }
    }
  }

  if (data & REQ_COM) {
    ReqInfo info = {
        .full_data.data_it.board_id = board_id,
        .full_data.info_t = COMMAND,
    };
    mex.dps_payload.data = info.raw_data;
    if (dps.send_f(&mex)) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

board_list_info *dps_master_list_board() {
  uint8_t len = c_vector_length(dps.boards);
  board_list_info *res =
      calloc(1, sizeof(board_list_info) + (sizeof(board_info) * len));
  res->board_num = len;
  for (uint8_t i = 0; i < len; i++) {
    board_record *board = c_vector_get_at_index(dps.boards, i);
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
int dps_master_list_vars(uint8_t board_id, var_list_info **o_list) {
  CHECK_INIT();
  CHECK_INPUT(o_list);

  board_record *board = c_vector_find(dps.boards, &board_id);
  var_list_info *list = NULL;
  if (board) {
    uint8_t len = c_vector_length(board->vars);
    *o_list = calloc(len, sizeof(*list) + (len * sizeof(*((*list).vars))));
    list = *o_list;

    for (uint8_t i = 0; i < len; i++) {
      var_record *var = c_vector_get_at_index(board->vars, i);
      if (var) {
        list->vars[i] = *var;
      }
    }
    (*o_list)->board_num = len;

    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

// INFO: return a list of all the coms known by the master in a board
int dps_master_list_coms(com_list_info **o_list) {
  CHECK_INIT();
  CHECK_INPUT(o_list);

  uint8_t len = c_vector_length(dps.coms);
  *o_list = calloc(1, sizeof(**o_list) + (len * sizeof(*(**o_list).coms)));
  com_list_info *list = *o_list;

  for (uint8_t i = 0; i < len; i++) {
    com_record *com = c_vector_get_at_index(dps.coms, i);
    if (!com) {
      free(list);
      return EXIT_FAILURE;
    }
    memcpy(list->coms[i].name, com->name, NAME_MAX_SIZE);
    list->coms[i].metadata.full_data = com->metadata.full_data;
  }
  list->board_num = len;

  return EXIT_SUCCESS;
}

// INFO: fetch the current value of a variable in a board in the system
int dps_master_refresh_value_var(uint8_t board_id, uint8_t var_id) {
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    var_record *var = c_vector_find(board->vars, &var_id);
    if (var) {
      return send_refresh_req_var(board_id, var);
    }
  }
  return EXIT_FAILURE;
}

// INFO: fetch the current value of all variables in a board in the system
int dps_master_refresh_value_var_all(uint8_t board_id) {
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    var_record *var = NULL;
    for (uint8_t i = 0; i < c_vector_length(board->vars); i++) {
      var = c_vector_get_at_index(board->vars, i);
      if (var) {
        return send_refresh_req_var(board_id, var);
      }
    }
  }
  return EXIT_FAILURE;
}

// INFO: fetch the current value of a variable in a board in the system and put
// in in o_buffer
int dps_master_get_value_var(uint8_t board_id, uint8_t var_i,
                             var_record *o_var) {
  board_record *board = c_vector_find(dps.boards, &board_id);
  if (board) {
    var_record *var = c_vector_find(board->vars, &var_i);
    if (var && var->updated) {
      *o_var = *var;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}

// INFO: send and update request for a variable of a board
// if the size is do not fit can message or is greater than the size of the
// variable the message will not be sent and return EXIT_FAILURE
int dps_master_update_var(uint8_t board_id, uint8_t var_id, void *value,
                          uint8_t value_size) {
  CHECK_INIT();
  CHECK_INPUT(value);
  CHECK_INPUT(value_size);

  board_record *board = c_vector_find(dps.boards, &board_id);
  if (!board) {
    return EXIT_FAILURE;
  }
  var_record *var = c_vector_find(board->vars, &var_id);
  if (!var) {
    return EXIT_FAILURE;
  }

  if (value_size > var->metadata.full_data.size) {
    return EXIT_FAILURE;
  }

  VariableModify new_value = {
      .full_data.obj_id = {.board_id = board_id, .data_id = var_id},
  };
  memcpy(new_value.full_data.value, value, value_size);
  CanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .dps_payload =
          {
              .mext_type = {SET_CURRENT_VAR_VALUE},
              .data = new_value.raw_data,
          },
  };

  return dps.send_f(&mex);
}

// INFO: send a command with a payload
// if value size is > then the bound of the command metadata or greater to CAN
// payload buffer message is not sent. If value is NULL or value_size is 0
// message is not sent
int dps_master_send_command(uint16_t com_dps_id, void *value,
                            uint8_t value_size) {
  com_record *com = c_vector_find(dps.coms, &com_dps_id);
  if (!com) {
    return EXIT_FAILURE;
  }

  if (com->metadata.full_data.size < value_size ||
      value_size > CAN_MAX_SIZE_MEX) {
    return EXIT_FAILURE;
  }

  CanMessage mex = {
      .id = com->metadata.full_data.com_id,
      .dlc = com->metadata.full_data.size,
  };
  memcpy(&mex.rawMex, value, value_size);

  return EXIT_SUCCESS;
}

int dps_master_check_mex_recv(CanMessage *mex) {
  CHECK_INIT();

  CHECK_INPUT(mex);

  if (mex->id == DPS_CAN_MESSAGE_ID) {
    switch (mex->dps_payload.mext_type.type) {
    case GET_BOARD_NAME:
      return get_board_name_exec(mex);
    case VAR_NAME:
      return get_var_name_exec(mex);
    case VAR_METADATA:
      return get_var_metadata_exec(mex);
    case COM_NAME:
      return get_com_name_exec(mex);
    case COM_METADATA:
      return get_com_metadata_exec(mex);
    case GET_CURRENT_VAR_VALUE:
      return get_curr_var_value_exec(mex);
    }
  }

  return EXIT_FAILURE;
}

// debug

int dps_master_print_boards() {
  int len = c_vector_length(dps.boards);
  for (int i = 0; i < len; i++) {
    board_record *board = c_vector_get_at_index(dps.boards, i);
    if (!board) {
      return EXIT_FAILURE;
    }
    printf("board id: %d,", board->id);
    printf("board name: %s\n", board->board_name);
  }

  return EXIT_SUCCESS;
}

int dps_master_print_coms() {
  c_vector_to_string(dps.coms);
  return 0;
}

int dps_master_print_vars() {
  uint8_t len = c_vector_length(dps.boards);
  for (uint8_t i = 0; i < len; i++) {
    board_record *board = c_vector_get_at_index(dps.boards, i);
    if (board) {
      uint8_t vars = c_vector_length(board->vars);
      for (uint8_t j = 0; j < vars; j++) {
        var_record *var = c_vector_get_at_index(board->vars, j);
        if (var) {
          printf("var name: %s,", var->name);
          printf("board id: %d,", var->metadata.full_data.obj_id.board_id);
          printf("var id: %d,", var->metadata.full_data.obj_id.data_id);
          printf("var size: %d,", var->metadata.full_data.size);
          printf("var signed: %d,", var->metadata.full_data.signe_num);
          printf("var float: %d,", var->metadata.full_data.float_num);
          printf("var value: %ld,", (long)*var->value);
          printf("var board: %s\n", board->board_name);
        }
      }
    }
  }
  return 0;
}
