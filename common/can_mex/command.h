#ifndef __DPS_CAN_COMMAND__
#define __DPS_CAN_COMMAND__

#include "base_mex_components/base_payload.h"
#include "object.h"

typedef ObjName CommandInfoName;

DPS_TYPEDEF(struct{
        ObjectId obj_id;
        uint8_t size;
        uint8_t min;
        uint8_t max;
        uint8_t float_num :1;
        uint8_t signe_num :1;
        }, CommandInfoMetadata);

DPS_TYPEDEF( 
        struct{
        uint8_t data[CAN_MAX_SIZE_MEX];
        },SendCommandPayload);

#endif // !__DPS_CAN_COMMAND__
