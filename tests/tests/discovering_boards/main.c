#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>


int main(void)
{
  NEW_BOARD()board_1 ={0};
  NEW_BOARD()board_2 ={0};
  NEW_BOARD()board_3 ={0};
  DpsMaster_h master = {0};

  const uint16_t slaves_id = 0x128;
  const uint16_t master_id = 0x129;

  if (dps_master_init(&master, master_id, slaves_id, can_send_test)<0) 
  {
    FAILED("init dps master failed");
  }

  if(dps_slave_init(&board_1.core.m_dps_slave, can_send_test, "board 1", 1, master_id, slaves_id)<0)
  {
    FAILED("init dps board 1");
  }

  if(dps_slave_init(&board_2.core.m_dps_slave, can_send_test, "board 2", 2, master_id, slaves_id)<0)
  {
    FAILED("init dps board 2");
  }

  if(dps_slave_init(&board_3.core.m_dps_slave, can_send_test, "board 3", 3, master_id, slaves_id)<0)
  {
    FAILED("init dps board 3");
  }
  
  if(start_board(&board_1.core)<0){
    FAILED("failed start board 1");
  }

  if(start_board(&board_2.core)<0){
    FAILED("failed start board 1");
  }

  if(start_board(&board_3.core)<0){
    FAILED("failed start board 1");
  }


  printf("cleaning\n");
  stop_board(&board_1.core);
  stop_board(&board_2.core);
  stop_board(&board_3.core);
  dps_master_destroy(&master);

  print_SCORE();
  return 0;
}
