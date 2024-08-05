#ifndef __DPS_CAN_BOARD__
#define __DPS_CAN_BOARD__

#include "base_mex_components/base_payload.h"
#include "object.h"

DPS_TYPEDEF(
        struct AssignBoardName{
        ObjectId obj_id;
        char name[CAN_MAX_SIZE_MEX - sizeof(obj_id)];
        },AssignBoardName);

DPS_TYPEDEF(
        typedef struct BoardName{
        char name[CAN_MAX_SIZE_MEX];
        },BoardName);

#endif // !__DPS_CAN_BOARD__
