#ifndef __DPS_MESSAGES__
#define __DPS_MESSAGES__

#include "can_mex/base_mex_components/base_payload.h"
#include "can_mex/base_mex_components/mex_types.h"

#define DPS_CAN_MESSAGE_ID 0x512

typedef struct DPSCanMessage {
  uint16_t id;
  uint8_t dlc;
  union {
    struct {
      mex_type_u8_t mext_type;
      DpsPayload data;
    } dps_payload;
    RawPayloadCanMex rawMex;
  } GenericPayload;
} DPSCanMessage;

typedef int (*can_send)(DPSCanMessage *mex);

#endif // !__DPS_MESSAGES__
