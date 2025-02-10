#include "messages.h"
#include <stdint.h>

//private

static int8_t _dps_message_slave_pack(const struct DpsSlaveMex* self, 
    uint64_t* const restrict o_buffer)
{
  register uint64_t x=0;
  register uint64_t i=0;

  x = ((uint8_t) self->Mode)& 0x0F;
  i|=x;

  x=  ((uint8_t) self->board_id) & 0x0F;
  x <<= 4;
  i|=x;

  switch (self->Mode)
  {
    case 0:
      x = ((uint32_t) self->Mode_0.name) & 0xFFFFFFFF;
      x <<= 8;
      i|=x;
      break;
    case 1:
      x = ((uint8_t) self->Mode_1.var_id) & 0x0F;
      x <<=8;
      i|=x;

      x = ((uint32_t) self->Mode_1.name) & 0xFFFFFFFF;
      x <<= 16;
      i|=x;
      break;
    case 2:
      x = ((uint8_t) self->Mode_2.var_id) & 0x0F;
      x <<=8;
      i|=x;

      x = ((uint8_t) self->Mode_2.type) & 0x03;
      x <<=12;
      i|=x;

      x = ((uint8_t) self->Mode_2.size) & 0x03;
      x <<=14;
      i|=x;
      break;
    case 3:
      x = ((uint8_t) self->Mode_3.var_id) & 0x0F;
      x <<=8;
      i|=x;

      x = ((uint32_t) self->Mode_3.value) & 0xFFFFFFFF;
      x <<= 16;
      i|=x;
      break;
  }

  *o_buffer = i;

  return 8;
}

static int8_t _dps_message_master_pack(const struct DpsMasterMex* self, 
    uint64_t* const restrict o_buffer)
{
  register uint64_t x=0;
  register uint64_t i=0;

  x = ((uint8_t) self->Mode)& 0x0F;
  i|=x;

  switch (self->Mode)
  {
    case 0:
      x = ((uint8_t) self->Mode_0.reserved) & 0x01;
      x <<=4;
      i|=x;
      break;
    case 1:
      x = ((uint8_t) self->Mode_1.board_id) & 0x0F;
      x <<=4;
      i|=x;
      break;
    case 2:
      x = ((uint8_t) self->Mode_2.board_id) & 0x0F;
      x <<=4;
      i|=x;

      x = ((uint8_t) self->Mode_2.var_id) & 0x0F;
      x <<=8;
      i|=x;

      break;
    case 3:
      x = ((uint8_t) self->Mode_3.board_id) & 0x0F;
      x <<=4;
      i|=x;

      x = ((uint8_t) self->Mode_3.var_id) & 0x0F;
      x <<=8;
      i|=x;

      x = ((uint32_t) self->Mode_3.var_id) & 0xFFFFFFFF;
      x <<=16;
      i|=x;
      break;
  }

  *o_buffer = i;

  return 8;
}

static int8_t _dps_message_slave_unpack(struct DpsSlaveMex* o_self, 
    const uint64_t buffer)
{
  register uint64_t x;
	register uint64_t i = (buffer);

  x = i & 0x0f;
  o_self->Mode = x;

  x = (i >> 4) & 0x0f;
  o_self->board_id = x;

  switch (o_self->Mode)
  {
    case 0:
      x = (i >> 8) & 0xFFFFFFFF;
      o_self->Mode_0.name = x;
      break;
    case 1:
      x = (i >> 8) & 0x0F;
      o_self->Mode_1.var_id = x;

      x = (i >> 16) & 0xFFFFFFFF;
      o_self->Mode_1.name = x;
      break;
    case 2:
      x = (i >> 8) & 0x0F;
      o_self->Mode_2.var_id = x;

      x = (i >> 12) & 0x02;
      o_self->Mode_2.type = x;

      x = (i >> 14) & 0x02;
      o_self->Mode_2.size = x;
      break;
    case 3:
      x = (i >> 8) & 0x0F;
      o_self->Mode_3.var_id = x;

      x = (i >> 16) & 0xFFFFFFFF;
      o_self->Mode_3.value = x;
      break;
  }

  return 0;
}

static int8_t _dps_message_master_unpack(struct DpsMasterMex* o_self, 
    const uint64_t buffer)
{
  register uint64_t x;
	register uint64_t i = (buffer);

  x = i & 0x0f;
  o_self->Mode = x;

  switch (o_self->Mode)
  {
    case 0:
      x = (i >> 4) & 0x01;
      o_self->Mode_0.reserved= x;
      break;
    case 1:
      x = (i >> 4) & 0x0F;
      o_self->Mode_1.board_id = x;
      break;
    case 2:
      x = (i >> 4) & 0x0F;
      o_self->Mode_2.board_id = x;

      x = (i >> 8) & 0x0F;
      o_self->Mode_2.var_id= x;
      break;
    case 3:
      x = (i >> 4) & 0x0F;
      o_self->Mode_3.board_id= x;

      x = (i >> 8) & 0x0F;
      o_self->Mode_3.var_id= x;

      x = (i >> 16) & 0xFFFFFFFF;
      o_self->Mode_3.value= x;
      break;
  }

  return 0;
}


//public
int8_t dps_messages_pack(struct DpsMessagesObj* self,
    enum DpsMessages mex_type,
    uint64_t* const restrict o_buffer)
{
  switch (mex_type)
  {
    case DPS_MESSAGES_SLAVE: return _dps_message_slave_pack(&self->dps_slave_mex, o_buffer);
    case DPS_MESSAGES_MASTER: return _dps_message_master_pack(&self->dps_master_mex, o_buffer);
  }
  return -1;
}
int8_t dps_messages_unpack(struct DpsMessagesObj* const restrict o_self,
    const enum DpsMessages mex_type,
    const uint64_t buffer)
{
  switch (mex_type)
  {
    case DPS_MESSAGES_SLAVE: return _dps_message_slave_unpack(&o_self->dps_slave_mex, buffer);
    case DPS_MESSAGES_MASTER: return _dps_message_master_unpack(&o_self->dps_master_mex, buffer);
  }
  return -1;
}
