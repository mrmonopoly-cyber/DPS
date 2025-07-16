#ifndef __DPS_MESSAGES__
#define __DPS_MESSAGES__

#include <stdint.h>
#include "dps_messages.h"

enum DPS_LENGHT{
  BOARD_NAME_LENGTH  =7,
  VAR_NAME_LENGTH = 6,
  COM_NAME_LENGTH = 6,
  DATA_ID_SIZE_BIT = 4,
};

enum DATA_GENERIC_TYPE {
    DATA_UNSIGNED=0,
    DATA_SIGNED,
    DATA_FLOATED,
};

enum DPS_PRIMITIVE_TYPES{
    DPS_TYPES_UINT8_T,
    DPS_TYPES_UINT16_T,
    DPS_TYPES_UINT32_T,
    DPS_TYPES_INT8_T,
    DPS_TYPES_INT16_T,
    DPS_TYPES_INT32_T,
    DPS_TYPES_FLOAT_T,
};

typedef struct{
  uint16_t id;
  uint8_t dlc;
  uint64_t full_word;
}DpsCanMessage;

typedef int8_t (*can_send) (const DpsCanMessage* const restrict);
typedef void(*wait_after_send) (void);
 
#endif // !__DPS_MESSAGES__
