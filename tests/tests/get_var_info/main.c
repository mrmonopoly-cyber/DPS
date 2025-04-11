#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>


int main(void)
{
  NEW_BOARD(
      uint8_t u8_th;
      int32_t s32_user;
      )board1 ={.u8_th = 12, .s32_user = 55};
  NEW_BOARD(float f_stw;)board2 ={.f_stw = 19.2f};
  NEW_BOARD()board3 ={0};
  MasterBoard_t master = {0};

  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  dps_slave_init(&board1.core.m_dps_slave, can_send_test, "2_v_b1", 1, master_id, slaves_id);
  dps_slave_init(&board2.core.m_dps_slave, can_send_test, "1_v_b1", 2, master_id, slaves_id);
  dps_slave_init(&board3.core.m_dps_slave, can_send_test, "0_v_b3", 3, master_id, slaves_id);
  dps_master_init(&master.m_dps_master, master_id, slaves_id, can_send_test);

  start_board(&board1.core);
  start_board(&board2.core);
  start_board(&board3.core);
  start_master_board(&master);

  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_UINT8_T, &board1.u8_th, NULL, "u8_th")<0,
      "board1 monitor u8_th");

  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_INT32_T, &board1.s32_user, NULL, "s32_user")<0,
      "board1 monitor s32_user");

  TEST_EXPR(
      dps_monitor_primitive_var(&board2.core.m_dps_slave, DPS_TYPES_FLOAT_T, &board2.f_stw, NULL, "vf_stw")<0,
      "board2 monitor vf_stw");

  dps_master_new_connection(&master.m_dps_master);
  sleep(1);

  BoardListInfo* boards = dps_master_list_board(&master.m_dps_master);
  if (boards && boards->board_num == 3)
  {
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      dps_master_request_info_board(&master.m_dps_master, boards->boards[i].id, REQ_VAR);
    }
    sleep(10);
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
          TEST_EXPR(vars, "board3 # of vars == 0");
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
