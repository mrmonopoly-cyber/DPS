#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

typedef enum{
  Board_1 = 1,
  Board_2 = 2,
  Board_3 = 3,
  
  __BOARD_COUNT
}BoardIndex;

#define FOR_ALL_BOARDS(board_index) \
  for(BoardIndex board_index = Board_1; board_index < __BOARD_COUNT; board_index++)

static NEW_BOARD(
    uint8_t u8_th;
    int32_t s32_user;
    )board1 ={.u8_th = 12, .s32_user = 55};

static NEW_BOARD(float f_stw;)board2 ={.f_stw = 19.2f};

static NEW_BOARD(
    uint64_t u64;
    int64_t i64;
    double f64;
    )board3 ={.u64 = 3, .i64=-2, .f64 = -6.7,};

static MasterBoard_t master = {0};


int _update_var(const BoardIndex board_id, const uint8_t var_id, const void* const raw_value,
    const uint8_t size)
{
  int err=0;

  if((err=dps_master_update_var(&master.m_dps_master, (uint8_t)board_id, var_id, raw_value, size))<0)
  {
    FAILED("sending update req with err:");
    printf("%d\n",err);
    return -1;
  }

  sleep(2);

  return dps_master_refresh_value_var(&master.m_dps_master, (uint8_t) board_id, var_id);
}

static void _print_boards_infos(const BoardIndex board)
{
  switch (board)
  {
    case Board_1:
      break;
      printf("Board: %d, var: %d, value: %d\n", Board_1, 0, board1.u8_th);
      printf("Board: %d, var: %d, value: %d\n", Board_1, 1, board1.s32_user);
      break;
    case Board_2:
      printf("Board: %d, var: %d, value: %f\n", Board_2, 0, board2.f_stw);
      break;
    case Board_3:
      printf("Board: %d, var: %d, value: %ld\n", Board_3, 0, board3.u64);
      printf("Board: %d, var: %d, value: %ld\n", Board_3, 1, board3.i64);
      printf("Board: %d, var: %d, value: %lf\n", Board_3, 2, board3.f64);
      break;
    case __BOARD_COUNT:
      break;
    }
}

int main(void)
{
  int err =0;
  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  FOR_ALL_BOARDS(board)
  {
    _print_boards_infos(board);
  }


  #define fast_dps_slave_init(board, name, id)\
    dps_slave_init(&board.core.m_dps_slave, can_send_test, wait_f, name, id, master_id, slaves_id);


  fast_dps_slave_init(board1, "2_v_b1", Board_1);
  fast_dps_slave_init(board2, "1_v_b1", Board_2);
  fast_dps_slave_init(board3, "0_v_b3", Board_3);
  dps_master_init(&master.m_dps_master, master_id, slaves_id, can_send_test, wait_f);

  start_board(&board1.core);
  start_board(&board2.core);
  start_board(&board3.core);
  start_master_board(&master);

  //INFO: BOARD 1
  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_UINT8_T, &board1.u8_th, NULL, "u8_th")<0,
      "board1 monitor u8_th");

  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_INT32_T, &board1.s32_user, NULL, "s32_user")<0,
      "board1 monitor s32_user");

  //INFO: BOARD 2
  TEST_EXPR(
      dps_monitor_primitive_var(&board2.core.m_dps_slave, DPS_TYPES_FLOAT_T, &board2.f_stw, NULL, "vf_stw")<0,
      "board2 monitor vf_stw");

  //INFO: BOARD 3
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_UINT64_T, &board3.u64, NULL, "u64th")<0,
      "board3 monitor u64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_INT64_T, &board3.i64, NULL, "i64th")<0,
      "board3 monitor i64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_DOUBLE_T, &board3.f64, NULL, "f64th")<0,
      "board3 monitor f64t");

  dps_master_new_connection(&master.m_dps_master);
  sleep(2);

  BoardListInfo* boards = dps_master_list_board(&master.m_dps_master);
  if (boards)
  {
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      if((err =dps_master_request_info_board(&master.m_dps_master,boards->boards[i].id,REQ_VAR))<0)
      {
        FAILED("failed refresh of board.");
        printf("board :%s, err code: %d\n",boards->boards[i].name,err);
      }
      sleep(5);
    }
    free(boards);
  }
  else
  {
    FAILED("boars not found");
  }
  sleep(3);

  #define TEST_UPDATE(VAR_TYPE, VAR_VALUE, BOARD, VAR_ID, OVAR, FMT)\
  {\
    const VAR_TYPE VAR_NAME = VAR_VALUE;\
    if(_update_var(BOARD, VAR_ID, &VAR_NAME, sizeof(VAR_NAME))<0)\
    {\
      FAILED("send update to "#BOARD " var " #VAR_ID);\
    }\
    else\
    {\
      PASSED("send update to "#BOARD " var "#VAR_ID);\
    }\
    sleep(2);\
    printf("values: current: "#FMT"expected: "#FMT"\n", OVAR, VAR_NAME);\
    if (OVAR != VAR_NAME)\
    {\
      FAILED("update to "#BOARD " var "#VAR_ID);\
    }\
    else\
    {\
      PASSED("update to "#BOARD " var "#VAR_ID);\
    }\
  }

  //INFO: Board 1
  TEST_UPDATE(uint8_t, 99, Board_1, 0, board1.u8_th, "%d");
  TEST_UPDATE(int32_t, -49, Board_1, 1, board1.s32_user, "%d");

  // //INFO: Board 2
  TEST_UPDATE(float, -29.5, Board_2, 0, board2.f_stw, "%f");

  // //INFO: Board 3
  TEST_UPDATE(uint64_t, 99887766, Board_3, 0, board3.u64, "%lu");
  TEST_UPDATE(int64_t, -11223344, Board_3, 1, board3.i64, "%ld");
  TEST_UPDATE(double, -4455.8866, Board_3, 2, board3.f64, "%lf");

  FOR_ALL_BOARDS(board)
  {
    _print_boards_infos(board);
  }

  printf("cleaning\n");

  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_master_board(&master);

  print_SCORE();
  return 0;
}
