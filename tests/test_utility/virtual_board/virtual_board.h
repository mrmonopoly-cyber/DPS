#ifndef __DPS_VIRTUAL_BOARD__
#define __DPS_VIRTUAL_BOARD__

#include <stdint.h>

#include "../../../src/slave/dps_slave.h"


typedef struct {
  DpsSlave_h m_dps_slave;
  uint8_t board_id;
}Board_t;

#define NEW_BOARD(board_name, vars)\
  struct{\
    Board_t core;\
    vars\
  }board_name;

#endif // !__DPS_VIRTUAL_BOARD__
