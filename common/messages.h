#ifndef __DPS_MESSAGES__
#define __DPS_MESSAGES__

#include "can_mex/base_mex_components/base_payload.h"
#include "can_mex/base_mex_components/mex_types.h"


typedef struct CanMessage{
    uint16_t id;
    uint8_t dlc;
    union{
        struct{
            mex_type_u8_t mext_type;
            DpsPayload data;
        }dps_payload;
        RawPayloadCanMex rawMex;
    }payload;
    uint8_t padding_byte;
}CanMessage;

uint8_t t = sizeof(CanMessage);

void init_new_mex(CanMessage* mex, MEX_TYPE dps_type, DpsPayload* dps_payload);
void init_new_raw_mex(CanMessage* mex, RawPayloadCanMex* raw_payload);

MEX_TYPE extract_message_type(CanMessage* mex);

#endif // !__DPS_MESSAGES__
