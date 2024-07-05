#ifndef __DPS_MASTER__
#define __DPS_MASTER__

#include "dps_common.h"

void dps_master_init(can_send send_f);

void dps_master_refresh();

uint8_t dps_master_check_can_mex_recv();

#endif // !__DPS_MASTER__
