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


int main(void)
{
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

  MasterBoard_t master = {0};

  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  dps_slave_init(&board1.core.m_dps_slave, can_send_test, wait_f, "2_v_b1", 1, master_id, slaves_id);
  dps_slave_init(&board2.core.m_dps_slave, can_send_test, wait_f, "1_v_b1", 2, master_id, slaves_id);
  dps_slave_init(&board3.core.m_dps_slave, can_send_test, wait_f, "0_v_b3", 3, master_id, slaves_id);
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
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_INT32_T, &board1.s32_user, NULL, "s32_user")<0,
      "board1 monitor s32_user");

  //BOARD 2
  TEST_EXPR(
      dps_monitor_primitive_var(&board2.core.m_dps_slave, DPS_TYPES_FLOAT_T, &board2.f_stw, NULL, "vf_stw")<0,
      "board2 monitor vf_stw");

  //BOARD 3
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_UINT64_T, &board3.u64, NULL, "u64th")<0,"board3 monitor u64t");
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_INT64_T, &board3.i64, NULL, "i64th")<0,"board3 monitor i64t");
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_DOUBLE_T, &board3.f64, NULL, "f64th")<0,"board3 monitor f64t");

  dps_master_new_connection(&master.m_dps_master);
  sleep(2);

  BoardListInfo* boards = dps_master_list_board(&master.m_dps_master);
  if (boards && boards->board_num == 3)
  {
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      dps_master_request_info_board(&master.m_dps_master, boards->boards[i].id, REQ_VAR);
    }
    sleep(5);
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      VarListInfo* vars = dps_master_list_vars(&master.m_dps_master, boards->boards[i].id);
      switch (boards->boards[i].id) {
        case 1:
          TEST_EXPR(!vars || vars->var_num != 2 , "board1 # of vars == 2");
          if (vars) {
            printf("vars num: %d\n",vars->var_num);
          }
          break;
        case 2:
          TEST_EXPR(!vars || vars->var_num != 1 , "board2 # of vars == 1");
          if (vars) {
            printf("vars num: %d\n",vars->var_num);
          }
          break;
        case 3:
          TEST_EXPR(!vars, "board3 # of vars == 3");
          if (vars) {
            printf("vars num: %d\n",vars->var_num);
          }
          break;
        default:
          FAILED("invalid var: ");
          printf("%d\n",i);
          break;
      }
      if (vars)
      {
        free(vars);
        vars=NULL;
      }
    }
  }
  else
  {
    FAILED("boards not found\n");
  }

  dps_master_print_vars(&master.m_dps_master);

  for (uint8_t i=0; i<boards->board_num; i++)
  {
    int err=0;
    TEST_EXPR((err = dps_master_refresh_value_var_all(&master.m_dps_master, boards->boards[i].id))<0,
        "refresh value board");
    printf("refreshed board: %s, with error: %d\n",boards->boards[i].name,err);
  }

  sleep(2);

  VarRecord var_value = {0};
  //board 1
  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, 1, 0, &var_value)<0, "get value of board 1 var id 0: 2_v_b1");

  TEST_EXPR(var_value.v_u32 != board1.u8_th, "board 1, u8_th: comparing recv value with expected one");
  printf("given: %d, expected: %d\n",var_value.v_u32,board1.u8_th);

  TEST_EXPR(memcmp(var_value.name, "u8_t", 5), "comparing name of var");
  printf("given %s, expected %s\n",var_value.name,"u8_t");

  TEST_EXPR(var_value.size!=0 || var_value.type != DATA_UNSIGNED, "comparing type var");


  //board 2

  //board 3
  memset(&var_value, 0, sizeof(var_value));

  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, 3, 0, &var_value)<0, "get value of board 3 var id 0: u64t");

  TEST_EXPR(
      var_value.size != 3 ||
      var_value.type != DATA_UNSIGNED ||
      strncmp(var_value.name, "u64t", strlen("u64t"))
      , "checking metadata of board3: u64t");
  printf("given name: %s, expected name: %s\n", var_value.name, "u64t");
  printf("given size: %d, expected size: %d\n", var_value.size, 3);
  printf("given type: %d, expected type: %d\n", var_value.type, DATA_UNSIGNED);

  TEST_EXPR(var_value.v_u64 != board3.u64, "board 3, u64: comparing recv value with expected one");
  printf("given: %ld, expected: %ld\n",var_value.v_u64,board3.u64);

  memset(&var_value, 0, sizeof(var_value));
  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, 3, 1, &var_value)<0, "get value of board 3 var id 1: i64t");
  
  TEST_EXPR(
      var_value.size != 3 ||
      var_value.type != DATA_SIGNED ||
      strncmp(var_value.name, "i64t", strlen("i64t"))
      , "checking metadata of board3: i64");
  printf("given name: %s, expected name: %s\n", var_value.name, "i64t");
  printf("given size: %d, expected size: %d\n", var_value.size, 3);
  printf("given type: %d, expected type: %d\n", var_value.type, DATA_SIGNED);

  TEST_EXPR(var_value.v_i64 != board3.i64, "board 3, i64: comparing recv value with expected one");
  printf("given: %ld, expected: %ld\n",var_value.v_i64,board3.i64);

  memset(&var_value, 0, sizeof(var_value));
  TEST_EXPR(dps_master_get_value_var(&master.m_dps_master, 3, 2, &var_value)<0, "get value of board 3 var id 2: f64t");

  TEST_EXPR(
      var_value.size != 3 ||
      var_value.type != DATA_FLOATED ||
      strncmp(var_value.name, "f64t", strlen("f64t"))
      , "checking metadata of board3: f64t");
  printf("given name: %s, expected name: %s\n", var_value.name, "f64t");
  printf("given size: %d, expected size: %d\n", var_value.size, 3);
  printf("given type: %d, expected type: %d\n", var_value.type, DATA_FLOATED);

  TEST_EXPR(var_value.v_f64 != board3.f64, "board 3, f64: comparing recv value with expected one");
  printf("given: %lf, expected: %lf\n",var_value.v_f64,board3.f64);

  
  if (boards)
  {
    free(boards);
  }

  printf("cleaning\n");

  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_master_board(&master);

  print_SCORE();
  return 0;
}
