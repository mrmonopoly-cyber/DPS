#include <due_can.h>

void setup() {
  Serial.begin(115200);

  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can0.begin(CAN_BPS_500K);
}

void loop() {
  CAN_FRAME mex;
  mex.id = 400;
  mex.extended = false;
  mex.length = 8;
  mex.data.value = 24;

  Can0.sendFrame(mex);
  delay(10);
}
