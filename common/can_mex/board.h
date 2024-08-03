#ifndef __DPS_CAN_BOARD__
#define __DPS_CAN_BOARD__

#include "definitions.h"
#include "objectId.h"

typedef struct AssignBoardName{
    ObjectId obj_id;
    char name[CAN_MAX_SIZE_MEX - sizeof(obj_id)];
}AssignBoardName;

typedef struct BoardName{
    char name[CAN_MAX_SIZE_MEX];
}BoardName;

void 
populate_mex_payload_board_name(RawPayloadCanMex* mex, char* name, uint8_t name_len);

void 
populate_mex_payload_assign_board_id(
                                    RawPayloadCanMex* mex, 
                                    char* name, 
                                    uint8_t name_len, 
                                    uint8_t id);

#endif // !__DPS_CAN_BOARD__
