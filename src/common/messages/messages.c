#include "messages.h"
#include "dps_template.h"
#include <stdint.h>

//private

static int8_t _dps_message_slave_pack(const struct DpsSlaveMex* self, 
    uint64_t* const restrict o_buffer)
{
  can_obj_dps_template_h_t o;

  switch (self->Mode)
  {
    case 0:
      o.can_0x28a_DpsSlaveMexMode0.Mode =0;
      o.can_0x28a_DpsSlaveMexMode0.board_id =self->board_id;
      o.can_0x28a_DpsSlaveMexMode0.board_name = self->Mode_0.name;
      return  pack_message(&o, CAN_ID_DPSSLAVEMEXMODE0, o_buffer);
    case 1:
      o.can_0x28b_DpsSlaveMexMode1.Mode =1;
      o.can_0x28b_DpsSlaveMexMode1.board_id =self->board_id;
      o.can_0x28b_DpsSlaveMexMode1.var_id =self->Mode_1.var_id;
      o.can_0x28b_DpsSlaveMexMode1.var_name =self->Mode_1.name;
      return pack_message(&o, CAN_ID_DPSSLAVEMEXMODE1, o_buffer);
    case 2:
      o.can_0x28c_DpsSlaveMexMode2.Mode = 2;
      o.can_0x28c_DpsSlaveMexMode2.board_id = self->board_id;
      o.can_0x28c_DpsSlaveMexMode2.var_id = self->Mode_2.var_id;
      o.can_0x28c_DpsSlaveMexMode2.size = self->Mode_2.size;
      o.can_0x28c_DpsSlaveMexMode2.type = self->Mode_2.type;
      return pack_message(&o, CAN_ID_DPSSLAVEMEXMODE2, o_buffer);
    case 3:
      o.can_0x28d_DpsSlaveMexMode3.Mode =3;
      o.can_0x28d_DpsSlaveMexMode3.board_id = self->board_id;
      o.can_0x28d_DpsSlaveMexMode3.var_id = self->Mode_3.var_id;
      o.can_0x28d_DpsSlaveMexMode3.value = self->Mode_3.value;
      return pack_message(&o, CAN_ID_DPSSLAVEMEXMODE3, o_buffer);
    default:
      return -1;
  }
}

static int8_t _dps_message_master_pack(const struct DpsMasterMex* self, 
    uint64_t* const restrict o_buffer)
{
  can_obj_dps_template_h_t o;

  switch (self->Mode)
  {
    case 0:
      o.can_0x294_DpsMasterMexMode0.Mode =0;
      o.can_0x294_DpsMasterMexMode0.reserved =0;
      return  pack_message(&o, CAN_ID_DPSMASTERMEXMODE0, o_buffer);
    case 1:
      o.can_0x295_DpsMasterMexMode1.Mode =1;
      o.can_0x295_DpsMasterMexMode1.board_id =self->Mode_1.board_id;
      return  pack_message(&o, CAN_ID_DPSMASTERMEXMODE1, o_buffer);
    case 2:
      o.can_0x296_DpsMasterMexMode2.Mode =2;
      o.can_0x296_DpsMasterMexMode2.board_id = self->Mode_2.board_id;
      o.can_0x296_DpsMasterMexMode2.var_id = self->Mode_2.var_id;
      return  pack_message(&o, CAN_ID_DPSMASTERMEXMODE2, o_buffer);
    case 3:
      o.can_0x297_DpsMasterMexMode3.Mode = 3;
      o.can_0x297_DpsMasterMexMode3.board_id = self->Mode_3.board_id;
      o.can_0x297_DpsMasterMexMode3.var_id = self->Mode_3.var_id;
      o.can_0x297_DpsMasterMexMode3.value = self->Mode_3.value;
      return  pack_message(&o, CAN_ID_DPSMASTERMEXMODE3, o_buffer);
    default:
      return -1;
  }
}

static int8_t _dps_message_slave_unpack(struct DpsSlaveMex* o_self, 
    const uint64_t buffer)
{
  const uint8_t x = buffer & 0x0f;
  can_obj_dps_template_h_t o;

  switch (x)
  {
    case 0:
      unpack_message(&o, CAN_ID_DPSSLAVEMEXMODE0, buffer, 8, 0);
      o_self->Mode=0;
      o_self->board_id = o.can_0x28a_DpsSlaveMexMode0.board_id;
      o_self->Mode_0.name = o.can_0x28a_DpsSlaveMexMode0.board_name;
      break;
    case 1:
      unpack_message(&o, CAN_ID_DPSSLAVEMEXMODE1, buffer, 8, 0);
      o_self->Mode =1;
      o_self->board_id = o.can_0x28b_DpsSlaveMexMode1.board_id;
      o_self->Mode_1.var_id = o.can_0x28b_DpsSlaveMexMode1.var_id;
      o_self->Mode_1.name = o.can_0x28b_DpsSlaveMexMode1.var_name;
      break;
    case 2:
      unpack_message(&o, CAN_ID_DPSSLAVEMEXMODE2, buffer, 8, 0);
      o_self->Mode =2;
      o_self->board_id = o.can_0x28c_DpsSlaveMexMode2.board_id;
      o_self->Mode_2.var_id = o.can_0x28c_DpsSlaveMexMode2.var_id;
      o_self->Mode_2.size = o.can_0x28c_DpsSlaveMexMode2.size;
      o_self->Mode_2.type = o.can_0x28c_DpsSlaveMexMode2.type;
      break;
    case 3:
      unpack_message(&o, CAN_ID_DPSSLAVEMEXMODE3, buffer, 8, 0);
      o_self->Mode =3;
      o_self->board_id = o.can_0x28d_DpsSlaveMexMode3.board_id;
      o_self->Mode_3.var_id = o.can_0x28d_DpsSlaveMexMode3.var_id;
      o_self->Mode_3.value = o.can_0x28d_DpsSlaveMexMode3.value;
      break;
    default:
      return -1;
  }

  return 0;
}

static int8_t _dps_message_master_unpack(struct DpsMasterMex* o_self, 
    const uint64_t buffer)
{
  const uint8_t x = buffer & 0x0f;
  can_obj_dps_template_h_t o;
  switch (x)
  {
    case 0:
      unpack_message(&o, CAN_ID_DPSMASTERMEXMODE0, buffer, 8, 0);
      o_self->Mode = 0;
      o_self->Mode_0.reserved =0;
      break;
    case 1:
      unpack_message(&o, CAN_ID_DPSMASTERMEXMODE1, buffer, 8, 0);
      o_self->Mode = 1;
      o_self->Mode_1.board_id = o.can_0x295_DpsMasterMexMode1.board_id;
      break;
    case 2:
      unpack_message(&o, CAN_ID_DPSMASTERMEXMODE2, buffer, 8, 0);
      o_self->Mode = 2;
      o_self->Mode_2.board_id = o.can_0x296_DpsMasterMexMode2.board_id;
      o_self->Mode_2.var_id = o.can_0x296_DpsMasterMexMode2.var_id;
      break;
    case 3:
      unpack_message(&o, CAN_ID_DPSMASTERMEXMODE3, buffer, 8, 0);
      o_self->Mode = 3;
      o_self->Mode_3.board_id = o.can_0x297_DpsMasterMexMode3.board_id;
      o_self->Mode_3.var_id = o.can_0x297_DpsMasterMexMode3.var_id;
      o_self->Mode_3.value = o.can_0x297_DpsMasterMexMode3.value;
      break;
    default:
      return -1;
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
