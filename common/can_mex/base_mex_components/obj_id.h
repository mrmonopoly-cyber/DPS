#ifndef __DPS_OBJ_ID__
#define __DPS_OBJ_ID__

#include <cstdint>

typedef struct ObjectId{
    uint8_t board_id : 4;
    uint8_t data_id : 4;
}ObjectId;

#endif // !__DPS_OBJ_ID__
