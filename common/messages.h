#ifndef __DPS_MESSAGES__
#define __DPS_MESSAGES__

#include "can_mex/base_mex_components/base_payload.h"
#include "can_mex/base_mex_components/mex_types.h"

#define DPS_CAN_MESSAGE_ID 0x512


typedef struct CanMessage{
    uint16_t id;
    uint8_t dlc;
    union{
        struct{
            mex_type_u8_t mext_type;
            DpsPayload data;
        }dps_payload;
        RawPayloadCanMex rawMex;
    };
    uint8_t padding_byte;
}CanMessage;

typedef uint8_t (*can_send) (CanMessage* mex);

uint8_t t = sizeof(CanMessage);

#endif // !__DPS_MESSAGES__
