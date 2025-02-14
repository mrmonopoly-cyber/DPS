#include "score_lib/test_lib.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"
#include "can/can.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define CAN_INTERFACE "dps_vcan"

int8_t can_send_test(const struct DpsCanMessage* const restrict self)
{
  static DpsCanInterface_h can_node = {0};
  if (!can_node.private_data[0])
  {
    dps_can_interface_init(&can_node, CAN_INTERFACE);
  }

  DpsCanInterfaceMex mex ={
    .id = self->id,
    .dlc = self->dlc,
    .data = self->full_word,
  };

  return dps_can_interface_send(&can_node, &mex);
}

int main(void)
{
  print_SCORE();
  return 0;
}
