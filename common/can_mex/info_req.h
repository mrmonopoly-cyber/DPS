#ifndef __DPS_CAN_INFO_REQ__
#define __DPS_CAN_INFO_REQ__

enum INFO_TYPES{
    VAR,
    BOARD,
    COMMAND,
    NEW_CONNECTION,
};

#include "base_mex_components/base_payload.h"
DPS_TYPEDEF(struct {
    enum INFO_TYPES info_t;
},ReqInfo);


#endif // !__DPS_CAN_INFO_REQ__
