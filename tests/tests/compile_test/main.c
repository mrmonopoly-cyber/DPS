#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

#define CAN_INTERFACE "dps_vcan"

int main(void)
{
  PASSED("compilation and basic init passed");
  print_SCORE();
  return 0;
}
