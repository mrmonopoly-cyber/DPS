#ifndef __DPS_MEX_TYPES__
#define __DPS_MEX_TYPES__

#include <stdint.h>

enum MEX_TYPE{
    GET_BOARD_NAME,
    SET_BOARD_NAME,

    GET_CURRENT_VAR_VALUE,
    SET_CURRENT_VAR_VALUE,

    GET_INFO,

    DUMMY_TYPE,
};

typedef struct{
    uint8_t type;
}mex_type_u8_t;

#endif // !__DPS_MEX_TYPES__
