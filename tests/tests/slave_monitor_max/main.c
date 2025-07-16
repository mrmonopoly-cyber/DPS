#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int main(void)
{
  DpsSlave_h slave = {0};
  const uint16_t master_id = 16;
  const uint16_t slaves_id = 18;
  uint8_t vars_to_monitor[16] = {0}; //INFO: fill the slave
  uint8_t invalid_var_to_monitor = 0;

  int err =0;
  if((err = dps_slave_init(&slave,can_send_test, wait_f, "board 1",1, master_id, slaves_id))<0)
  {
    FAILED("slave init failed");
    printf("err: %d\n",err);
  }

  dps_slave_start(&slave);


  for (uint8_t i=0; i<sizeof(vars_to_monitor)/sizeof(*vars_to_monitor); i++)
  {
    char str[32] = "valid vars to fill slave: ";
    sprintf(str + 25, "%d", i);
    TEST_EXPR(
        dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &vars_to_monitor[i], NULL, str)<0,
        str);
  }

    TEST_EXPR(
        dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &invalid_var_to_monitor, NULL, "invalid var")>=0,
        "invalid var");


  print_SCORE();
  return 0;
}
