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

#define MONITOR_VAR_CHECK(exp, var_name)\
  if(exp<0)\
  {\
    FAILED("failed monitor var: "var_name);\
    return -1;\
  }\
  else\
  {\
    PASSED("passed monitor var: "var_name);\
  }

static int8_t _can_send_test(const struct DpsCanMessage* const restrict self)
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

  DpsSlave_h slave;
  const uint16_t master_id = 16;
  const uint16_t slaves_id = 16;

  if(dps_slave_init(&slave,_can_send_test, "board 1",1, master_id, slaves_id)<0)
  {
    FAILED("slave init failed");
  }

  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &no_update_fun.u8_var, NULL, "u8 var no update fun"), "u8 var no update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT16_T, &no_update_fun.u16_var, NULL, "u16 var no update fun"), "u16 var no update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT32_T, &no_update_fun.u32_var, NULL, "u32 var no update fun"), "u32 var no update fun");

  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT8_T, &no_update_fun.i8_var, NULL, "i8 var no update fun"), "i8 var no update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT16_T, &no_update_fun.i16_var, NULL, "i16 var no update fun"), "i16 var no update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT32_T, &no_update_fun.i32_var, NULL, "i32 var no update fun"), "i32 var no update fun");

  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT8_T, &yes_update_fun.u8_var, NULL, "u8 var yes update fun"), "u8 var yes update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT16_T, &yes_update_fun.u16_var, NULL, "u16 var yes update fun"), "u16 var yes update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_UINT32_T, &yes_update_fun.u32_var, NULL, "u32 var yes update fun"), "u32 var yes update fun");

  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT8_T, &yes_update_fun.i8_var, NULL, "i8 var yes update fun"), "i8 var yes update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT16_T, &yes_update_fun.i16_var, NULL, "i16 var yes update fun"), "i16 var yes update fun");
  MONITOR_VAR_CHECK(dps_monitor_primitive_var(&slave, DPS_TYPES_INT32_T, &yes_update_fun.i32_var, NULL, "i32 var yes update fun"), "i32 var yes update fun");

  printf("destroying slave\n");
  dps_slave_destroy(&slave);

  print_SCORE();
  return 0;
}
