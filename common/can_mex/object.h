#ifndef __DPS_OBJECT__
#define __DPS_OBJECT__

#include <stdint.h>

#include "base_mex_components/mex_types.h"
#include "base_mex_components/base_payload.h"
#include "base_mex_components/obj_id.h"

DPS_TYPEDEF(
        struct{
        ObjectId obj_id;
        char name[CAN_MAX_SIZE_MEX];
        }, ObjName);

DPS_TYPEDEF(
        struct{
        ObjectId obj_id;
        uint8_t size;
        uint8_t min;
        uint8_t max;
        uint8_t float_num :1;
        uint8_t signe_num :1;
        }, ObjMetadata);

#endif // !__DPS_OBJECT__
