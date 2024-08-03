#ifndef __DPS_CAN_COMMAND__
#define __DPS_CAN_COMMAND__

#include "definitions.h"

typedef ObjName CommandInfoName;
typedef ObjMetadata CommandInfoMetadata;
typedef struct SendCommandPayload{
    uint8_t data[CAN_MAX_SIZE_MEX];
}SendCommandPayload;

#define populate_mex_payload_var_name(mex,name,name_len) \
    populate_mex_payload_name(mex,name,name_len)

#define populate_mex_payload_com_metadata(\
                            mex, \
                            size, \
                            min,\
                            max,\
                            is_float,\
                            is_signed)\
    populate_mex_payload_metadata(mex,size,min,max,is_float,is_signed)


#endif // !__DPS_CAN_COMMAND__
