#include <due_can.h>

#include "lib/DPS/dps_slave.h"

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
}

void loop() {}
