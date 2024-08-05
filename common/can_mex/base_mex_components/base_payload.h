#ifndef __DPS_RAW_PAYLOAD__
#define __DPS_RAW_PAYLOAD__

#include <cstdint>

#include "mex_types.h"
#include "obj_id.h"

#define CAN_PROTOCOL_MAX_PAYLOAD_SIZE 8
#define CAN_MAX_SIZE_MEX (CAN_PROTOCOL_MAX_PAYLOAD_SIZE - sizeof(MEX_TYPE))

typedef struct{
    uint8_t raw_buffer[CAN_PROTOCOL_MAX_PAYLOAD_SIZE];
}RawPayloadCanMex;

typedef struct{
    uint8_t raw_buffer[CAN_PROTOCOL_MAX_PAYLOAD_SIZE - sizeof(MEX_TYPE)];

}DpsPayload;

#define DPS_TYPEDEF(T,name) \
    typedef union{ \
        T full_data; \
        DpsPayload raw_data;\
    }name;

#endif // !__DPS_RAW_PAYLOAD__
