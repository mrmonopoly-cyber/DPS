#ifndef __PDS_CAN_DEFINITIONS__
#define __PDS_CAN_DEFINITIONS__

#include "objectId.h"
#include <cstdint>

#define CAN_PROTOCOL_MAX_PAYLOAD_SIZE 8

enum MEX_TYPE{
    GET_BOARD_NAME,
    SET_BOARD_NAME,

    GET_CURRENT_VAR_VALUE,
    SET_CURRENT_VAR_VALUE,
    GET_INFO_VAR_NAME,
    GET_INFO_VAR_METADATA,

    REQ_INFO_VAR,
    REQ_INFO_COMMAND,
};

typedef struct RawPayloadCanMex{
    uint8_t raw_buffer[CAN_PROTOCOL_MAX_PAYLOAD_SIZE];
}RawPayloadCanMex;

typedef struct BaseMexDps{
    uint8_t mex_type;
}BaseMexDps;

#define CAN_MAX_SIZE_MEX (CAN_PROTOCOL_MAX_PAYLOAD_SIZE - sizeof(BaseMexDps))

typedef struct ObjName{
    ObjectId obj_id;
    char name[CAN_MAX_SIZE_MEX - sizeof(obj_id)];
}ObjName;

typedef struct ObjMetadata{
    ObjectId obj_id;
    uint8_t size;
    uint8_t min;
    uint8_t max;
    uint8_t float_num :1;
    uint8_t signe_num :1;
}ObjMetadata;

void 
populate_mex_payload_name(RawPayloadCanMex* mex, char* name, uint8_t name_len);

void 
populate_mex_payload_metadata(
                            RawPayloadCanMex* mex, 
                            uint8_t size, 
                            uint8_t min,
                            uint8_t max,
                            bool is_float,
                            bool is_signed);

#endif // !__PDS_CAN_DEFINITIONS__
