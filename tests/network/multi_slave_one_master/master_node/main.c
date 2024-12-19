#include <unistd.h>
#define DEBUG
#include "../../../../dps_master.h"
#include "../../../test_lib.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>
#include <string.h>

#include <pthread.h>

int can_socket = -1;

int send_f_can(DPSCanMessage *mex) {
  struct can_frame f = {
      .can_dlc = mex->dlc,
      .can_id = mex->id,
  };
  memcpy(f.data, mex->GenericPayload.rawMex.raw_buffer, mex->dlc);
  return can_send_frame(can_socket, &f);
}

void *check_incomming_message(void *args) {
  struct can_frame mex_lib = {0};
  while (1) {
    if (!can_recv_frame(can_socket, &mex_lib)) {
      DPSCanMessage mex = {
          .dlc = mex_lib.can_dlc,
          .id = mex_lib.can_id,
      };
      memcpy(mex.GenericPayload.rawMex.raw_buffer, mex_lib.data,
             mex_lib.can_dlc);
      if (dps_master_check_mex_recv(&mex)) {
        FAILED("failed processing message");
        return NULL;
      }
    }
  };
  return NULL;
}

int main(void) {
  can_socket = can_init("test_can_dps");
  if (can_socket < 0) {
    printf("error init can\n");
    goto free;
  }
  dps_master_init(send_f_can);

  pthread_t new_thread = 1;
  pthread_create(&new_thread, NULL, check_incomming_message, NULL);

  if (dps_master_new_connection()) {
    FAILED("failed new connection slaves");
    goto free;
  }
  PASSED("new connection slaves ok");

  sleep(5);

  if (dps_master_print_boards()) {
    FAILED("failed print boards");
    goto free;
  }
  PASSED("ok print boards");

  if (dps_master_request_info_board(0, REQ_VAR)) {
    FAILED("failed request var info");
    goto free;
  }
  PASSED("passed request var info");

  sleep(5);
  var_list_info *vars = NULL;
  if (dps_master_list_vars(0, &vars)) {
    FAILED("failed getting list of vars");
    goto free;
  }

  for (int i = 0; i < vars->board_num; i++) {
    var_record *var = &vars->vars[i];
    printf("var index: %d,var name: %s\t", i, var->name);
    if (var->metadata.full_data.float_num) {
      printf("var float: %f\n", *(float *)var->value);
      continue;
    }
    if (var->metadata.full_data.signe_num) {
      switch (var->metadata.full_data.size) {
      case 1:
        printf("var int8: %d\n", *(int8_t *)var->value);
        break;
      case 2:
        printf("var int16: %d\n", *(int16_t *)var->value);
        break;
      case 4:
        printf("var int32: %d\n", *(int32_t *)var->value);
        break;
      default:
        FAILED("invalid size signed");
        goto free;
        break;
      }
      continue;
    }
    switch (var->metadata.full_data.size) {
    case 1:
      printf("var uint8: %d\n", *(uint8_t *)var->value);
      break;
    case 2:
      printf("var uint16: %d\n", *(uint16_t *)var->value);
      break;
    case 4:
      printf("var uint32: %d\n", *(uint32_t *)var->value);
      break;
    default:
      FAILED("invalid size unsigned");
      goto free;
    }
  }

  for (int i = 0; i < vars->board_num; i++) {
    uint8_t new_value = 26;
    var_record *var = &vars->vars[i];
    if (dps_master_update_var(0, var->metadata.full_data.obj_id.data_id,
                              &new_value, sizeof(new_value))) {
      FAILED("failed update request");
      goto free;
    }
  }

  sleep(5);
  {
    var_list_info *vars = NULL;
    if (dps_master_list_vars(0, &vars)) {
      FAILED("failed getting list of vars");
      goto free;
    }

    for (int i = 0; i < vars->board_num; i++) {
      var_record *var = &vars->vars[i];
      printf("var index: %d,var name: %s\t", i, var->name);
      if (var->metadata.full_data.float_num) {
        printf("var float: %f\n", *(float *)var->value);
        continue;
      }
      if (var->metadata.full_data.signe_num) {
        switch (var->metadata.full_data.size) {
        case 1:
          printf("var int8: %d\n", *(int8_t *)var->value);
          break;
        case 2:
          printf("var int16: %d\n", *(int16_t *)var->value);
          break;
        case 4:
          printf("var int32: %d\n", *(int32_t *)var->value);
          break;
        default:
          FAILED("invalid size signed");
          goto free;
          break;
        }
        continue;
      }
      switch (var->metadata.full_data.size) {
      case 1:
        printf("var uint8: %d\n", *(uint8_t *)var->value);
        break;
      case 2:
        printf("var uint16: %d\n", *(uint16_t *)var->value);
        break;
      case 4:
        printf("var uint32: %d\n", *(uint32_t *)var->value);
        break;
      default:
        FAILED("invalid size unsigned");
        goto free;
      }
    }
  }

  if (dps_master_refresh_value_var_all(0)) {
    FAILED("failed refresh all variables");
    goto free;
  }

  sleep(5);

  if (dps_master_print_vars()) {
    FAILED("failed print vars debug");
    goto free;
  }
  PASSED("passed print vars debug");
  if (dps_master_print_coms()) {
    FAILED("failed print coms debug");
    goto free;
  }
  PASSED("passed print coms debug");

  pthread_join(new_thread, NULL);

free:
  if (vars) {
    free(vars);
  }
  return 0;
}
