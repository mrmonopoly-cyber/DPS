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
  NEW_BOARD()board1 ={0};
  NEW_BOARD()board2 ={0};
  NEW_BOARD()board3 ={0};
  NEW_BOARD()board_excluded_1 ={0};
  NEW_BOARD()board_excluded_2 ={0};
  MasterBoard_t master = {0};
  MasterBoard_t master_unsed = {0};

  const uint16_t slaves_id = 0x128;
  const uint16_t master_id = 0x149;

  if (dps_master_init(&master.m_dps_master, master_id, slaves_id, can_send_test)<0) 
  {
    FAILED("init dps master failed");
  }

  if (dps_master_init(&master_unsed.m_dps_master, master_id+1, slaves_id+1, can_send_test)<0) 
  {
    FAILED("init dps master failed");
  }

  if(dps_slave_init(&board1.core.m_dps_slave, can_send_test, wait_f, "board1", 1, master_id, slaves_id)<0)
  {
    FAILED("init dps board1");
  }

  if(dps_slave_init(&board2.core.m_dps_slave, can_send_test, wait_f, "board2", 2, master_id, slaves_id)<0)
  {
    FAILED("init dps board2");
  }

  if(dps_slave_init(&board3.core.m_dps_slave, can_send_test, wait_f, "board3", 3, master_id, slaves_id)<0)
  {
    FAILED("init dps board3");
  }

  if(dps_slave_init(&board_excluded_1.core.m_dps_slave, can_send_test, wait_f, "excb_0", 0, 1, 2)<0)
  {
    FAILED("init dps excb_0");
  }

  if(dps_slave_init(&board_excluded_2.core.m_dps_slave, can_send_test, wait_f, "excb_2", 1, 4, 2)<0)
  {
    FAILED("init dps excb_2");
  }

  
  if(start_board(&board1.core)<0)
  {
    FAILED("failed start board1");
  }

  if(start_board(&board2.core)<0)
  {
    FAILED("failed start board2");
  }

  if(start_board(&board3.core)<0)
  {
    FAILED("failed start board3");
  }

  if(start_board(&board_excluded_1.core)<0)
  {
    FAILED("failed start board1");
  }

  if(start_board(&board_excluded_2.core)<0)
  {
    FAILED("failed start board2");
  }


  if (start_master_board(&master)<0)
  {
    FAILED("failed start master board");
  }

  if (start_master_board(&master_unsed)<0)
  {
    FAILED("failed start master board unused");
  }

  TEST_EXPR(dps_master_new_connection(&master.m_dps_master)<0,"master new connection request sent");
  TEST_EXPR(dps_master_new_connection(&master_unsed.m_dps_master)<0,"master unused new connection request sent");
  sleep(1);
  BoardListInfo* boards =  dps_master_list_board(&master.m_dps_master);
  if (boards)
  {
    printf("boards: %d\n",boards->board_num);
    dps_master_print_boards(&master.m_dps_master);
  }
  else
  {
    printf("boards: NULL\n");
  }
  TEST_EXPR(!boards || boards->board_num != 3, "board discovered success");
  #define BOARD_SEARCH(num,b_name) \
    for (uint8_t i=0; i<boards->board_num; i++)\
    {\
      if (boards->boards[i].id==num)\
      {\
        TEST_EXPR(strcmp(boards->boards[i].name,b_name),b_name " discovered");\
        printf("given: id: %d, name: %s\n", boards->boards[i].id, boards->boards[i].name);\
      }\
    }\

  if (boards)
  {
    BOARD_SEARCH(1,"board1");
    BOARD_SEARCH(2,"board2");
    BOARD_SEARCH(3,"board3");
    uint8_t found =0;
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      if (boards->boards[i].id == 0 && !strcmp(boards->boards[i].name, "excb0"))
      {
        found|=1;
      }

      if (boards->boards[i].id == 1 && !strcmp(boards->boards[i].name, "excb2"))
      {
        found|=2;
      }
    }
    if (found & 1) {
      FAILED("excluded board excb1 added");
    }
    if( found & 2){
      FAILED("excluded board excb2 added");
    }
    if (!found)
    {
      PASSED("excluded board not added");
    }
    free(boards);
  }


  boards = dps_master_list_board(&master_unsed.m_dps_master);
  TEST_EXPR(boards, "unused master list board");
  if (boards)
  {
    printf("boards master unused len: %d\n",boards->board_num);
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      printf("board: %i, id: %d, name: %s\n",i,boards->boards[i].id,boards->boards[i].name);
    }
    free(boards);
    boards=NULL;
  }


  printf("cleaning\n");
  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_board(&board_excluded_1.core);
  stop_board(&board_excluded_2.core);
  stop_master_board(&master);
  stop_master_board(&master_unsed);

  print_SCORE();
  return 0;
}
