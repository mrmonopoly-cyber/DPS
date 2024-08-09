#ifndef __DPS_CAN_BOARD__
#define __DPS_CAN_BOARD__

#include "base_mex_components/base_payload.h"
#include "base_mex_components/obj_id.h"
#include "object.h"

#define BOARD_NAME_LENGTH CAN_MAX_SIZE_MEX - sizeof(ObjectId)

DPS_TYPEDEF(
        struct {
        ObjectId obj_id;
        char name[BOARD_NAME_LENGTH];
        },AssignBoarId);

DPS_TYPEDEF(
        struct{
        char name[CAN_MAX_SIZE_MEX];
        },BoardName);

#endif // !__DPS_CAN_BOARD__
