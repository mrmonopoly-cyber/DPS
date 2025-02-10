#ifndef __DPS_CAN_MESSAGES__
#define __DPS_CAN_MESSAGES__

#include <stdint.h>

enum DpsMessages{
  DPS_MESSAGES_SLAVE=0,
  DPS_MESSAGES_MASTER,
};

struct DpsSlaveMex
{
  uint8_t Mode; //0,4
  uint8_t board_id; //4,4

  struct SlaveMode0{//Mode = 0 board name
    uint64_t name; //8,56
  }Mode_0;

  struct SlaveMode1{//Mode = 1 var name
    uint8_t var_id;//8,4
    uint64_t name;//16,48
  }Mode_1;

  struct SlaveMode2{//Mode = 2 var metadata 
    uint8_t var_id;//8,4
    uint8_t type;//12,2
    uint8_t size;//14,3
  }Mode_2;

  struct SlaveMode3{//Mode = 3 var value
   uint8_t var_id;//8,4
   uint32_t value;//16,32
  }Mode_3;
};

struct DpsMasterMex{
  uint8_t Mode; //0,4

  struct MasterMode0{//Mode = 0 Discover Boards
    uint8_t reserved; //4,1
  }Mode_0;

  struct MasterMode1{//Mode = 1 Request Infos Board
    uint8_t board_id; //4,4
  }Mode_1;

  struct MasterMode2{//Mode = 2 Request var value Board
    uint8_t board_id;//4,4
    uint8_t var_id;//8,4
  }Mode_2;

  struct MasterMode3{ //Mode = 3 Update var value Board
    uint8_t board_id;//4,4
    uint8_t var_id;//8,4
    uint32_t value;//16,32
  }Mode_3;
};

struct DpsMessagesObj{
  struct DpsSlaveMex dps_slave_mex;
  struct DpsMasterMex dps_master_mex;
};

int8_t
dps_messages_pack(struct DpsMessagesObj* const restrict self,
    const enum DpsMessages mex_type,
    const uint64_t* const restrict o_buffer);

int8_t
dps_messages_unpack(struct DpsMessagesObj* const restrict o_self,
    const enum DpsMessages mex_type,
    const uint64_t* const restrict buffer);

#endif // !__DPS_CAN_MESSAGES__
