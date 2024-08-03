#ifndef __DPS_CAN_VARIABLE__
#define __DPS_CAN_VARIABLE__

#include "objectId.h"
#include "definitions.h"

typedef struct VariableValue{
    ObjectId obj_id;
    uint8_t value[CAN_MAX_SIZE_MEX - sizeof(obj_id)];
}VariableValue;
typedef struct VariableValue VariableModify;

typedef ObjName VariableInfoName;
typedef ObjMetadata VariableInfoMetadata;

#define populate_mex_payload_var_name(mex,name,name_len) \
    populate_mex_payload_name(mex,name,name_len)

#define populate_mex_payload_var_metadata(\
                            mex, \
                            size, \
                            min,\
                            max,\
                            is_float,\
                            is_signed)\
    populate_mex_payload_metadata(mex,size,min,max,is_float,is_signed)

void 
populate_mex_payload_var_value(RawPayloadCanMex* mex, uint8_t* value, uint8_t value_size);

#endif // !__DPS_CAN_VARIABLE__
