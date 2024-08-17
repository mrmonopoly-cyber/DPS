#ifndef __DPS_OBJ_ID__
#define __DPS_OBJ_ID__

#define BOARD_ID_SIZE_BIT 3
#define DATA_ID_SIZE_BIT 5

#include <stdint.h>

typedef struct ObjectId {
  uint8_t board_id : BOARD_ID_SIZE_BIT;
  uint8_t data_id : DATA_ID_SIZE_BIT;
} ObjectId;

#endif // !__DPS_OBJ_ID__
