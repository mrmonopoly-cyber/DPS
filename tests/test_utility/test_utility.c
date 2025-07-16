#include "test_utility.h"
#include "can/can.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <threads.h>
#include <unistd.h>

int8_t can_send_test_full(const DpsCanMessage* const restrict self,
    const char* const can_interaface)
{
  DpsCanInterface_h can_node = {0};
  dps_can_interface_init(&can_node, can_interaface);

  DpsCanInterfaceMex mex ={
    .id = self->id,
    .dlc = self->dlc,
    .data = self->full_word,
  };

  int err = dps_can_interface_send(&can_node, &mex);
  dps_can_interface_shutdown(&can_node);

  return err;
}

int8_t can_send_test(const DpsCanMessage* const restrict self)
{
  return can_send_test_full(self, CAN_INTERFACE);
}

void wait_f(void)
{
  sleep(1);
}

static int _board_loop_slave(void* board)
{
  Board_t* const restrict self = board;
  DpsCanInterfaceMex mex = {0};
  DpsCanMessage dps_mex = {0};

  while (self->running)
  {
    if(dps_can_interface_read(&self->m_can_interface, &mex)>0)
    {
      dps_mex.dlc = mex.dlc;
      dps_mex.full_word = mex.data;
      dps_mex.id = mex.id;
      dps_slave_check_can_command_recv(&self->m_dps_slave, &dps_mex);
    }
  }
  dps_can_interface_shutdown(&self->m_can_interface);
  return 0;
}

static int _board_loop_master(void* board)
{
  MasterBoard_t* const restrict self = board;
  DpsCanInterfaceMex mex = {0};
  DpsCanMessage dps_mex = {0};

  while (self->running)
  {
    if(dps_can_interface_read(&self->m_can_interface, &mex)>0)
    {
      dps_mex.dlc = mex.dlc;
      dps_mex.full_word = mex.data;
      dps_mex.id = mex.id;
      dps_master_check_mex_recv(&self->m_dps_master, &dps_mex);
    }
  }
  dps_can_interface_shutdown(&self->m_can_interface);
  return 0;
}

int8_t start_board_full(Board_t* const board, const char* const can_interface)
{
  if(dps_can_interface_init(&board->m_can_interface, can_interface)<0)
  {
    return -1;
  }

  dps_slave_start(&board->m_dps_slave);
  board->running=1;
  printf("starting board\n");
  thrd_create(&board->thread, _board_loop_slave, board);
  return 0;
}

int8_t start_board(Board_t* const board)
{
  return start_board_full(board, CAN_INTERFACE);
}

int8_t stop_board(Board_t* const board)
{
  printf("stopping board: %ld\n",board->thread);
  board->running=0;
  thrd_join(board->thread,NULL);
  printf("dps slave destroying\n");
  dps_slave_destroy(&board->m_dps_slave);
  printf("dps slave destroying done\n");
  return 0;
}

int8_t start_master_board_full(MasterBoard_t* const board, const char* const can_interface)
{
  if(dps_can_interface_init(&board->m_can_interface, can_interface)<0)
  {
    return -1;
  }

  board->running=1;
  printf("starting master board\n");
  thrd_create(&board->thread, _board_loop_master, board);
  return 0;
}

int8_t start_master_board(MasterBoard_t* const board)
{
  return start_master_board_full(board, CAN_INTERFACE);
}

int8_t stop_master_board(MasterBoard_t* const board)
{
  printf("stopping board: %ld\n",board->thread);
  board->running=0;
  thrd_join(board->thread,NULL);
  dps_master_destroy(&board->m_dps_master);
  return 0;
}
