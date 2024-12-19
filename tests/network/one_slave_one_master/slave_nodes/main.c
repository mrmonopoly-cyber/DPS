#define DEBUG

#include "../../../../dps_slave.h"
#include "../../../test_lib.h"
#include "../../can_lib/canlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

int can_socket = -1;

int check_input_mex(void *args) {
  while (1) {
    struct can_frame frame;
    DPSCanMessage dps_mex;
    if (can_recv_frame(can_socket, &frame)) {
      return EXIT_FAILURE;
    }

    dps_mex.id = frame.can_id;
    dps_mex.dlc = frame.can_dlc;
    memcpy(dps_mex.GenericPayload.rawMex.raw_buffer, frame.data, frame.can_dlc);
    dps_check_can_command_recv(&dps_mex);
    printf("message\n");
  }
}

int send_f_can(DPSCanMessage *mex) {
  struct can_frame frame = {
      .can_dlc = mex->dlc,
      .can_id = mex->id,
  };
  memcpy(frame.data, mex->GenericPayload.rawMex.raw_buffer, mex->dlc);
  if (can_send_frame(can_socket, &frame)) {
    printf("failed send\n");
  }
  return 0;
  return 0;
}

uint8_t u8 = 1;
uint16_t u16 = 2;
uint32_t u32 = 3;

int8_t s8 = -1;
int16_t s16 = -2;
int32_t s32 = -3;

float fl = 5.2f;

uint8_t u8_1 = 1;
uint16_t u16_1 = 2;
uint32_t u32_1 = 3;

int8_t s8_1 = -1;
int16_t s16_1 = -2;
int32_t s32_1 = -3;

float fl_1 = 5.2f;

int main(void) {
  can_socket = can_init("test_can_dps");
  if (can_socket < 0) {
    printf("error init can\n");
    return -1;
  }

  BoardName board_name = {
      .full_data.name = "SLAVE",
  };
  if (dps_init(send_f_can, &board_name)) {
    FAILED("failed init");
    return -1;
  }
  PASSED("init ok");

  dps_slave_start();

#define MON_VAR(VAR, fun)                                                      \
  {                                                                            \
    VariableInfoPrimitiveType pr = {                                           \
        .name = #VAR,                                                          \
        .var_ptr = &VAR,                                                       \
    };                                                                         \
    if (fun(&pr)) {                                                            \
      FAILED("failed monitor " #VAR);                                          \
      return -1;                                                               \
    }                                                                          \
    PASSED("monitor ok var " #VAR);                                            \
  }

  MON_VAR(u8, dps_monitor_var_uint8_t);
  MON_VAR(u16, dps_monitor_var_uint16_t);
  MON_VAR(u32, dps_monitor_var_uint32_t);

  MON_VAR(s8, dps_monitor_var_int8_t);
  MON_VAR(s16, dps_monitor_var_int16_t);
  MON_VAR(s32, dps_monitor_var_int32_t);

  MON_VAR(fl, dps_monitor_var_float_t);

  MON_VAR(u8_1, dps_monitor_var_uint8_t);
  MON_VAR(u16_1, dps_monitor_var_uint16_t);
  MON_VAR(u32_1, dps_monitor_var_uint32_t);

  MON_VAR(s8_1, dps_monitor_var_int8_t);
  MON_VAR(s16_1, dps_monitor_var_int16_t);
  MON_VAR(s32_1, dps_monitor_var_int32_t);

  MON_VAR(fl_1, dps_monitor_var_float_t);

  printf("slave ready\n");

  thrd_t th;
  thrd_create(&th, check_input_mex, NULL);

  print_SCORE();

  dps_print_var();

  thrd_join(th, NULL);

  return 0;
}
