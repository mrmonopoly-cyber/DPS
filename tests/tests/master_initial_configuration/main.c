#include "score_lib/test_lib.h"
#include "src/master/dps_master.h"
#include "can/can.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define CAN_INTERFACE "dps_vcan"

#define CHECK_TEST(exp, exp_info) \
  if(exp)\
  {\
    FAILED(exp_info);\
  }\
  else\
  {\
    PASSED(exp_info);\
  }

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
  DpsMaster_h master = {0};

  const uint16_t master_id = 0x128;
  const uint16_t slaves_id = 0x129;

  CHECK_TEST(dps_master_init(&master, master_id, slaves_id, can_send_test), "master init");
  CHECK_TEST(dps_master_init(&master, master_id, slaves_id, can_send_test) == -1, "master double init failed");

  CHECK_TEST(dps_master_list_board(&master), "master info board empty");
  CHECK_TEST(dps_master_list_vars(&master,0), "master info vars empty");


  printf("cleaning\n");
  dps_master_destroy(&master);

  print_SCORE();
  return 0;
}
