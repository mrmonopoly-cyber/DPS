#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int main(void)
{
  DpsMaster_h master = {0};

  const uint16_t master_id = 0x128;
  const uint16_t slaves_id = 0x129;

  TEST_EXPR(dps_master_init(&master, master_id, slaves_id, can_send_test), "master init");
  TEST_EXPR(dps_master_init(&master, master_id, slaves_id, can_send_test) == -1, "master double init failed");

  TEST_EXPR(dps_master_list_board(&master), "master info board empty");
  TEST_EXPR(dps_master_list_vars(&master,0), "master info vars empty");


  printf("cleaning\n");
  dps_master_destroy(&master);

  print_SCORE();
  return 0;
}
