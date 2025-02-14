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
  DpsMaster_h master;
  DpsSlave_h slave;
  const uint16_t master_id = 16;
  const uint16_t slaves_id = 16;

  if(dps_master_init(&master, master_id, slaves_id, can_send_test)<0)
  {
    FAILED("master init failed");
  }

  if(dps_slave_init(&slave,can_send_test, "board 1",1, master_id, slaves_id)<0)
  {
    FAILED("slave init failed");
  }


  printf("destroying slave\n");
  dps_slave_destroy(&slave);
  printf("destroying master\n");
  dps_master_destroy(&master);

  PASSED("compilation and basic init passed");
  print_SCORE();
  return 0;
}
