#ifndef __DPS_CAN_COMMAND__
#define __DPS_CAN_COMMAND__

#include "base_mex_components/base_payload.h"
#include "object.h"

typedef ObjName CommandInfoName;
typedef ObjMetadata CommandInfoMetadata;

DPS_TYPEDEF( 
        struct{
        uint8_t data[CAN_MAX_SIZE_MEX];
        },SendCommandPayload);

#endif // !__DPS_CAN_COMMAND__
