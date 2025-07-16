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
  int err =0;
  NEW_BOARD(
      uint8_t u8_th;
      int32_t s32_user;
      )board1 ={.u8_th = 12, .s32_user = 55};
  NEW_BOARD(float f_stw;)board2 ={.f_stw = 19.2f};
  NEW_BOARD()board3 ={0};
  MasterBoard_t master = {0};


  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  dps_slave_init(&board1.core.m_dps_slave, can_send_test, wait_f, "2_v_b1", 1, master_id, slaves_id);
  dps_slave_init(&board2.core.m_dps_slave, can_send_test, wait_f, "1_v_b1", 2, master_id, slaves_id);
  dps_slave_init(&board3.core.m_dps_slave, can_send_test, wait_f, "0_v_b3", 3, master_id, slaves_id);
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
    }
    free(boards);
    sleep(3);
  }
  else
  {
    FAILED("boars not found");
  }
  sleep(3);

  if((err = dps_master_print_vars(&master.m_dps_master))<0)
  {
    FAILED("print vars failed: ");
    printf("err code: %d\n",err);
  }
  const uint8_t new_value = 99;
  dps_master_update_var(&master.m_dps_master, 1, 0, &new_value, sizeof(new_value));
  sleep(3);

  dps_master_refresh_value_var(&master.m_dps_master, 1, 0);
  sleep(3);

  {
    VarRecord v_rec = {0};
    if((err =dps_master_get_value_var(&master.m_dps_master, 1, 0, &v_rec))<0)
    {
      FAILED("get value var after update failed");
      printf("err code: %d\n",err);
    }

    TEST_EXPR(v_rec.v_u32!= new_value, "test the update of the variable:");
    printf("given: %d, expected %d\n",v_rec.v_u32,new_value);
  }

  {
    dps_master_refresh_value_var(&master.m_dps_master, 2, 0);
    sleep(2);
    VarRecord v_rec = {0};
    if((err =dps_master_get_value_var(&master.m_dps_master, 2, 0, &v_rec))<0)
    {
      FAILED("get value var after update failed");
      printf("err code: %d\n",err);
    }

    TEST_EXPR(v_rec.v_float!= 19.2f, "test the update of the variable:");
    printf("given: %f, expected %f\n",v_rec.v_float,19.2f);
  }

  printf("cleaning\n");

  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_master_board(&master);

  print_SCORE();
  return 0;
}
