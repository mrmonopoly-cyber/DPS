#include <cstdlib>
#include <cstring>
#include <due_can.h>

#include "lib/DPS/dps_slave.h"

uint8_t u8 = 1;
uint16_t u16 = 1;
uint32_t u32 = 1;
int8_t s8 = 1;
int16_t s16 = 1;
int32_t s32 = 1;
float float_v = 1;

static int send(CanMessage *mex) {
  CAN_FRAME mex_f;
  mex_f.id = mex->id;
  mex_f.length = mex->dlc;
  memcpy(&mex_f.data.value, mex->rawMex.raw_buffer, mex->dlc);

  Can0.sendFrame(mex_f);
}

void setup() {
  Serial.begin(115200);
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can0.begin(CAN_BPS_500K);

  BoardName b_name = {
      "SLAVE",
  };
  if (dps_init(send, &b_name)) {
    Serial.println("init dps failed");
    for (;;) {
    }
  }

#define SAVE_VAR(VAR, fun)                                                     \
  {                                                                            \
    VariableInfoPrimitiveType var;                                             \
    memcpy((void *)var.name, (void *)&VAR, sizeof(VAR));                       \
    var.var_ptr = &VAR;                                                        \
    if (fun(&var)) {                                                           \
      for (;;) {                                                               \
        Serial.println(String("failed monitor") + String(#VAR));               \
      }                                                                        \
    }                                                                          \
  }

  SAVE_VAR(u8, dps_monitor_var_uint8_t);
  SAVE_VAR(u16, dps_monitor_var_uint16_t);
  SAVE_VAR(u32, dps_monitor_var_uint32_t);
  SAVE_VAR(s8, dps_monitor_var_int8_t);
  SAVE_VAR(s16, dps_monitor_var_int16_t);
  SAVE_VAR(s32, dps_monitor_var_int32_t);
  SAVE_VAR(float_v, dps_monitor_var_float_t);
}

void loop() {
  if (Can0.available()) {
    CAN_FRAME in_mex;
    Can0.read(in_mex);
    Serial.print("mex received: " + String(in_mex.id) + ",");
    Serial.print(in_mex.data.low);
    Serial.println(in_mex.data.high);
    CanMessage d_mex;
    d_mex.id = in_mex.id;
    d_mex.dlc = in_mex.length;
    memcpy(d_mex.rawMex.raw_buffer, &in_mex.data.value, in_mex.length);
    dps_check_can_command_recv(&d_mex);
  }
}
