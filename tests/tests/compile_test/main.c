#include "score_lib/test_lib.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int8_t can_send_test(const struct DpsCanMessage* const restrict self __attribute_maybe_unused__)
{
  return 0;
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
