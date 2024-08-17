#ifndef __DPS_CAN_COMMAND__
#define __DPS_CAN_COMMAND__

#include "base_mex_components/base_payload.h"
#include "base_mex_components/obj_id.h"
#include "object.h"

#define COMMAND_NAME_SIZE CAN_MAX_SIZE_MEX - sizeof(uint16_t)

DPS_TYPEDEF(
    struct {
      uint16_t com_id;
      char name[COMMAND_NAME_SIZE];
    },
    CommandInfoName)

DPS_TYPEDEF(
    struct {
      uint16_t com_id;
      uint8_t size;
      uint8_t float_num : 1;
      uint8_t signe_num : 1;
      uint8_t board_id : BOARD_ID_SIZE_BIT;
    },
    CommandInfoMetadata);

DPS_TYPEDEF(struct { uint8_t data[CAN_MAX_SIZE_MEX]; }, SendCommandPayload);

#endif // !__DPS_CAN_COMMAND__
