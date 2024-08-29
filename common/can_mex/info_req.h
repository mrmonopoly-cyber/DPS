#ifndef __DPS_CAN_INFO_REQ__
#define __DPS_CAN_INFO_REQ__

#include "base_mex_components/obj_id.h"
enum INFO_TYPES{
    VAR,
    VAR_VALUE,
    COMMAND,
};

#include "base_mex_components/base_payload.h"
DPS_TYPEDEF(struct {
    ObjectId data_it;
    uint8_t info_t;
},ReqInfo);


#endif // !__DPS_CAN_INFO_REQ__
