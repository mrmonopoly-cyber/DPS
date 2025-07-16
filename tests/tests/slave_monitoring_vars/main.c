#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>


int8_t _post_update(const char *const, const void *const)
{
  printf("post update fn\n");
  return 0;
}


int main(void)
{
  typedef struct {
    uint8_t u8_var;
    uint16_t u16_var;
    uint32_t u32_var;
    uint64_t u64_var;

    int8_t i8_var;
    int16_t i16_var;
    int32_t i32_var;
    int64_t i64_var;

    float float_var;
  }FullVarSet;

  FullVarSet no_update_fun={0};
  FullVarSet yes_update_fun={0};

  DpsSlave_h slave = {0};
  const uint16_t master_id = 16;
  const uint16_t slaves_id = 18;

  int err =0;
  if((err = dps_slave_init(&slave,can_send_test, wait_f, "board 1",1, master_id, slaves_id))<0)
  {
    FAILED("slave init failed");
    printf("err: %d\n",err);
  }

  dps_slave_start(&slave);

  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &no_update_fun.u8_var, NULL, "u8_np")<0, "u8 var no update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT16_T, &no_update_fun.u16_var, NULL, "u16np")<0, "u16 var no update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT32_T, &no_update_fun.u32_var, NULL, "u32np")<0, "u32 var no update fun");

  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT8_T, &no_update_fun.i8_var, NULL, "i8_np")<0, "i8 var no update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT16_T, &no_update_fun.i16_var, NULL, "i16np")<0, "i16 var no update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT32_T, &no_update_fun.i32_var, NULL, "i32np")<0, "i32 var no update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_FLOAT_T, &yes_update_fun.float_var, NULL, "f_vnp")<0, "float var no update fun");

  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &yes_update_fun.u8_var, _post_update, "u8_np")<0, "u8 var yes update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT16_T, &yes_update_fun.u16_var, _post_update, "u16np")<0, "u16 var yes update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT32_T, &yes_update_fun.u32_var, _post_update, "u32np")<0, "u32 var yes update fun");

  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT8_T, &yes_update_fun.i8_var, _post_update, "i8_np")<0, "i8 var yes update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT16_T, &yes_update_fun.i16_var, _post_update, "i16np")<0, "i16 var yes update fun");
  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_INT32_T, &yes_update_fun.i32_var, _post_update, "i32np")<0, "i32 var yes update fun");

  TEST_EXPR(dps_monitor_primitive_var(&slave, DPS_TYPES_FLOAT_T, &yes_update_fun.float_var, _post_update, "f_vnp")<0, "float var yes update fun");
  dps_print_var(&slave);

  printf("destroying slave\n");
  dps_slave_destroy(&slave);


  print_SCORE();
  return 0;
}
