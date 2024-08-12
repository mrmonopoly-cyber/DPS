#ifndef __DPS_CAN_INFO_REQ__
#define __DPS_CAN_INFO_REQ__

enum INFO_TYPES{
    VAR,
    COMMAND,
};

#include "base_mex_components/base_payload.h"
DPS_TYPEDEF(struct {
    uint8_t board_id;
    enum INFO_TYPES info_t;
},ReqInfo);


#endif // !__DPS_CAN_INFO_REQ__
