#ifndef _DPS_TEST_UTILITY_
#define _DPS_TEST_UTILITY_

#include <stdint.h>
#include <threads.h>

#include "../../src/slave/dps_slave.h"
#include "../../src/master/dps_master.h"
#include "score_lib/test_lib.h"
#include "can/can.h"

#define CAN_INTERFACE "dps_vcan"
int8_t can_send_test_full(const DpsCanMessage* const restrict self,
    const char* const can_interaface);

int8_t can_send_test(const DpsCanMessage* const restrict self);

void wait_f(void);


typedef struct {
  DpsSlave_h m_dps_slave;
  thrd_t thread;
  uint8_t running:1;
  DpsCanInterface_h m_can_interface;
}Board_t;

typedef struct {
  DpsMaster_h m_dps_master;
  thrd_t thread;
  uint8_t running:1;
  DpsCanInterface_h m_can_interface;
}BoardMaster_t;

typedef struct {
  DpsMaster_h m_dps_master;
  thrd_t thread;
  uint8_t running:1;
  DpsCanInterface_h m_can_interface;
}MasterBoard_t;

#define NEW_BOARD(vars)\
  struct{\
    Board_t core;\
    vars\
  }

int8_t start_board_full(Board_t* const board, const char* const can_interface);
int8_t start_board(Board_t* const board);
int8_t stop_board(Board_t* const board);

int8_t start_master_board_full(MasterBoard_t* const board, const char* const can_interface);
int8_t start_master_board(MasterBoard_t* const board);
int8_t stop_master_board(MasterBoard_t* const board);

#endif // !_DPS_TEST_UTILITY_

