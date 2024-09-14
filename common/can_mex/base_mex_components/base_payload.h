#ifndef __DPS_RAW_PAYLOAD__
#define __DPS_RAW_PAYLOAD__

#include <stdint.h>

#include "mex_types.h"
#include "obj_id.h"

#define CAN_PROTOCOL_MAX_PAYLOAD_SIZE 8
#define CAN_MAX_SIZE_MEX (CAN_PROTOCOL_MAX_PAYLOAD_SIZE - sizeof(mex_type_u8_t))

typedef union{
    uint8_t raw_buffer[CAN_PROTOCOL_MAX_PAYLOAD_SIZE];
    uint32_t raw_words[CAN_PROTOCOL_MAX_PAYLOAD_SIZE/sizeof(uint32_t)];
    uint64_t full_buffer[CAN_PROTOCOL_MAX_PAYLOAD_SIZE/CAN_PROTOCOL_MAX_PAYLOAD_SIZE];
}RawPayloadCanMex;

typedef struct{
    uint8_t raw_buffer[CAN_MAX_SIZE_MEX];
}DpsPayload;

#define DPS_TYPEDEF(T,name) \
    typedef union{ \
        T full_data; \
        DpsPayload raw_data;\
    }name;

#endif // !__DPS_RAW_PAYLOAD__
