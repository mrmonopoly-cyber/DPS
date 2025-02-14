#include "can.h"
#include "can_lib/canlib.h"
#include <linux/can.h>
#include <string.h>

struct DpsCanInterface_t{
  int can_fd;
};

union DpsCanInterface_h_t_conv{
  DpsCanInterface_h* const hidden;
  struct DpsCanInterface_t* const clear;
};

#ifdef DEBUG
char __assert_size_dps_can[(sizeof(DpsCanInterface_h)==sizeof(struct DpsCanInterface_t))?1:-1];
#endif /* ifdef DEBUG */

//public

int8_t
dps_can_interface_init(DpsCanInterface_h* const restrict self,
    const char* can_interface)
{
  union DpsCanInterface_h_t_conv conv = {self};
  struct DpsCanInterface_t* const p_self = conv.clear;
  p_self->can_fd = can_init(can_interface);

  if (!p_self->can_fd)
  {
    return -1;
  }
  return 0;
}

int8_t
dps_can_interface_send(DpsCanInterface_h* const restrict self,
    const DpsCanInterfaceMex* const restrict mex)
{
  union DpsCanInterface_h_t_conv conv = {self};
  struct DpsCanInterface_t* const p_self = conv.clear;
  struct can_frame frame ={
    .can_dlc = mex->dlc,
    .can_id = mex->id,
  };

  memcpy(frame.data, &mex->data, sizeof(mex->data));

  return can_send_frame(p_self->can_fd, &frame);
}

int8_t 
dps_can_interface_read(DpsCanInterface_h* const restrict self,
    DpsCanInterfaceMex* const restrict o_mex)
{
  union DpsCanInterface_h_t_conv conv = {self};
  struct DpsCanInterface_t* const p_self = conv.clear;
  struct can_frame frame={0};


  can_recv_frame(p_self->can_fd, &frame);

  memset(o_mex, 0, sizeof(*o_mex));
  o_mex->id = frame.can_id;
  o_mex->dlc = frame.can_dlc;
  memcpy(&o_mex->data, frame.data, sizeof(frame.data));

  return 0;
}
