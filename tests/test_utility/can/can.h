#ifndef __DPS_CAN_INTERFACE__
#define __DPS_CAN_INTERFACE__

#include <stdint.h>

typedef struct __attribute__((aligned(4))){
  const uint8_t private_data[4];
}DpsCanInterface_h;

typedef struct{
  uint16_t id;
  uint8_t dlc;
  uint64_t data;
}DpsCanInterfaceMex;

int8_t
dps_can_interface_init(DpsCanInterface_h* const restrict self,
    const char* can_interface);

int8_t
dps_can_interface_send(DpsCanInterface_h* const restrict self,
    const DpsCanInterfaceMex* const restrict mex);

int8_t 
dps_can_interface_read(DpsCanInterface_h* const restrict self,
    DpsCanInterfaceMex* const restrict o_mex);

int8_t 
dps_can_interface_shutdown(DpsCanInterface_h* const restrict self);

#endif // !__DPS_CAN_INTERFACE__
