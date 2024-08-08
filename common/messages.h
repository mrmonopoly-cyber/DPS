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
    };
    uint8_t padding_byte;
}CanMessage;

uint8_t t = sizeof(CanMessage);

void init_new_mex_payload_dps(CanMessage* mex, enum MEX_TYPE dps_type, DpsPayload* dps_payload);
void init_new_mex_payload_raw(CanMessage* mex, RawPayloadCanMex* raw_payload);

enum MEX_TYPE extract_message_type(CanMessage* mex);

#endif // !__DPS_MESSAGES__
