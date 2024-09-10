#define DEBUG
#include "../../../../dps_slave.h"
#include "../../../test_lib.h"
#include "../../can_lib/canlib.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int can_socket = -1;

int send_f_can(CanMessage *mex) {
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
      CanMessage mex = {
          .dlc = mex_lib.can_dlc,
          .id = mex_lib.can_id,
      };
      memcpy(mex.GenericPayload.rawMex.raw_buffer, mex_lib.data,
             mex_lib.can_dlc);
      dps_check_can_command_recv(&mex);
    }
  };
  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("invalid input number %d\n\
                argv[1] = board_name of size <= 6\n\
                argv[2] = num of vars\n\
                argv[3] = num of coms\n",
           argc);
    return -1;
  }

  if (strlen(argv[2]) > 6) {
    printf("invalid board name %s\n\
                max len: %ld\n \
                given: %ld\n",
           argv[2], NAME_MAX_SIZE, strlen(argv[2]));
    return -2;
  }

  can_socket = can_init("test_can_dps");
  if (can_socket < 0) {
    printf("error init can\n");
    return -3;
  }

  char *board_name_orig = argv[1];
  uint8_t num_vars = atoi(argv[2]);
  uint8_t num_coms = atoi(argv[3]);

  BoardName board_name = {};
  memcpy(board_name.full_data.name, board_name_orig, NAME_MAX_SIZE);
  dps_init(send_f_can, &board_name);

  dps_slave_start();

  pthread_t new_thread = 1;
  pthread_create(&new_thread, NULL, check_incomming_message, NULL);

  VariableInfoPrimitiveType var = {
      .name = "vU1_X",
      .var_ptr = NULL,
  };

  uint8_t vars_u8[num_vars] = {};
  int16_t vars_u16[num_vars] = {};
  uint32_t vars_u32[num_vars] = {};
  int8_t vars_s8[num_vars] = {};
  int16_t vars_s16[num_vars] = {};
  int32_t vars_s32[num_vars] = {};
  float vars_float[num_vars] = {};

  for (int i = 0; i < num_vars; i++) {
    vars_u8[i] = i;
    var.name[1] = 'u';
    var.name[2] = '1';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_u8[i];
    if (dps_monitor_var_uint8_t(&var)) {
      FAILED("failed monitor u8");
      return -1;
    }

    vars_u16[i] = i;
    var.name[1] = 'u';
    var.name[2] = '2';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_u16[i];
    if (dps_monitor_var_uint16_t(&var)) {
      FAILED("failed monitor u16");
      return -1;
    }

    vars_u32[i] = i;
    var.name[1] = 'u';
    var.name[2] = '3';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_u32[i];
    if (dps_monitor_var_uint32_t(&var)) {
      FAILED("failed monitor u16");
      return -1;
    }

    vars_s8[i] = i;
    var.name[1] = 's';
    var.name[2] = '1';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_s8[i];
    if (dps_monitor_var_int8_t(&var)) {
      FAILED("failed monitor s8");
      return -1;
    }

    vars_s16[i] = i;
    var.name[1] = 's';
    var.name[2] = '2';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_s16[i];
    if (dps_monitor_var_int16_t(&var)) {
      FAILED("failed monitor s16");
      return -1;
    }

    vars_s32[i] = i;
    var.name[1] = 's';
    var.name[2] = '4';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_s32[i];
    if (dps_monitor_var_uint32_t(&var)) {
      FAILED("failed monitor s32");
      return -1;
    }

    vars_float[i] = i;
    var.name[1] = 'f';
    var.name[2] = '4';
    var.name[4] = '1' + i;
    var.var_ptr = &vars_float[i];
    if (dps_monitor_var_float_t(&var)) {
      FAILED("failed monitor float");
      return -1;
    }
  }

  dps_print_var();

  pthread_join(new_thread, NULL);

  return 0;
}
