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
  

  __MAX__BOARD
}BoardIndex;

NEW_BOARD(
    uint8_t u8_th;
    int32_t s32_user;
    )board1 ={.u8_th = 12, .s32_user = 55};

NEW_BOARD(
    float f_stw;
    )board2 ={.f_stw = 19.2f};

NEW_BOARD(
    uint64_t u64;
    int64_t i64;
    double f64;
    )board3 ={.u64 = 12, .i64= -200, .f64 = -3.14};


int main(void)
{
  MasterBoard_t master = {0};

  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  #define fast_dps_slave_init(board, name, id)\
    dps_slave_init(&board.core.m_dps_slave, can_send_test, wait_f, name, id, master_id, slaves_id);

  fast_dps_slave_init(board1, "2_v_b1", Board_1);
  fast_dps_slave_init(board2, "1_v_b1", Board_2);
  fast_dps_slave_init(board3, "3_v_b1", Board_3);

  dps_master_init(&master.m_dps_master, master_id, slaves_id, can_send_test, wait_f);

  start_board(&board1.core);
  start_board(&board2.core);
  start_board(&board3.core);
  start_master_board(&master);


  //BOARD 1
  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_UINT8_T, &board1.u8_th, NULL, "u8_th")<0,
      "board1 monitor u8_th");

  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_INT32_T, &board1.s32_user, NULL, "s32_u")<0,
      "board1 monitor s32_user");

  //BOARD 2
  TEST_EXPR(
      dps_monitor_primitive_var(&board2.core.m_dps_slave, DPS_TYPES_FLOAT_T, &board2.f_stw, NULL, "vf_st")<0,
      "board2 monitor vf_stw");

  //BOARD 3
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_UINT64_T, &board3.u64, NULL, "u64th")<0,
      "board3 monitor u64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_INT64_T, &board3.i64, NULL, "i64th")<0,
      "board3 monitor i64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_DOUBLE_T, &board3.f64, NULL, "f64th")<0,
      "board3 monitor f64t");


  if(dps_master_new_connection(&master.m_dps_master)<0)
  {
    FAILED("master could not connect");
    return -1;
  }

  sleep(3);

  BoardListInfo* boards = dps_master_list_board(&master.m_dps_master);
  int err=0;
  if (boards)
  {
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      const uint8_t board_id = boards->boards[i].id;
      printf("refresing info about board: %d\n",board_id);
      if((err =dps_master_request_info_board(&master.m_dps_master, board_id, REQ_VAR))<0)
      {
        FAILED("failed refresh of board.");
        printf("board :%s, err code: %d\n",boards->boards[i].name,err);
      }
    }
    sleep(6);
  }

  for(uint8_t i= 0; i< boards->board_num; i++ )
  {
    const uint8_t board_id = boards->boards[i].id;
    printf("refresing values about board: %d\n",board_id);
    dps_master_refresh_value_var_all(&master.m_dps_master, (uint8_t)board_id);
    sleep(5);
  }

  VarRecord records[6];
  memset(records, 0, sizeof(records));

  //INFO: board 1

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_1, 0, &records[0])<0, 
      "get: board 1, var id 0");

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_1, 1, &records[1])<0, 
      "get: board 1, var id 1");

  //INFO: board 2

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_2, 0, &records[2])<0, 
      "get: board 2, var id 0");

  //INFO: board 3

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_3, 0, &records[3])<0, 
      "get: board 3, var id 0");

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_3, 1, &records[4])<0, 
      "get: board 3, var id 1");

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, Board_3, 2, &records[5])<0, 
      "get: board 3, var id 2");


  //INFO: board 1
  TEST_EXPR(records[0].v_u8 != board1.u8_th, "check: board 1, u8_th");
  printf("expected: %d, given: %d\n", board1.u8_th, records[0].v_u8);
  TEST_EXPR(records[1].v_i32 != board1.s32_user , "check: board 1, s32_u");
  printf("expected: %d, given: %d\n", board1.s32_user, records[1].v_i32);

  //INFO: board 2
  TEST_EXPR(records[2].v_f32!= board2.f_stw, "check: board 2, f_stw");
  printf("expected: %f, given: %f\n", board2.f_stw, records[2].v_f32);

  //INFO: board 3
  TEST_EXPR(records[3].v_u64 != board3.u64, "check: board 3, u64");
  printf("expected: %lu, given: %lu\n", board3.u64, records[3].v_u64);
  TEST_EXPR(records[4].v_i64 != board3.i64, "check: board 3, i64");
  printf("expected: %ld, given: %ld\n", board3.i64, records[4].v_i64);
  TEST_EXPR(records[5].v_f64 != board3.f64, "check: board 3, f64");
  printf("expected: %lf, given: %lf\n", board3.f64, records[5].v_f64);

  if (boards)
  {
    free(boards);
    boards=NULL;
  }

  //INFO: cleaning
  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_master_board(&master);

  print_SCORE();
  return 0;
}
