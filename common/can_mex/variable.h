#ifndef __DPS_CAN_VARIABLE__
#define __DPS_CAN_VARIABLE__

#include "base_mex_components/base_payload.h"
#include "base_mex_components/obj_id.h"
#include "object.h"

DPS_TYPEDEF(
        struct {
        ObjectId obj_id;
        union{
            uint8_t value[CAN_MAX_SIZE_MEX - sizeof(ObjectId)];
        };
        },VariableValue);

DPS_TYPEDEF(struct{
        ObjectId obj_id;
        uint8_t size;
        uint8_t float_num :1;
        uint8_t signe_num :1;
        }, VariableInfoMetadata);
typedef VariableValue VariableModify;
typedef ObjName VariableInfoName;

#endif // !__DPS_CAN_VARIABLE__
