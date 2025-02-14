#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../common/common.h"

typedef int8_t (*post_update) (const char* const var_name, const void* const var);

typedef struct DpsSlave_h{
    const uint8_t private_data[32];
}DpsSlave_h;

// INFO: create dps manager
// send_f : function to send through CAN the data to the external controller
int8_t
dps_slave_init(DpsSlave_h* const restrict self,
        const can_send send_f,
        const char board_name[BOARD_NAME_LENGTH],
        const uint8_t dps_board_id,
        const uint16_t dps_can_id_master,
        const uint16_t dps_can_id_slaves);

int8_t
dps_slave_start(DpsSlave_h* const restrict self);

int8_t
dps_slave_disable(DpsSlave_h* const restrict self);

int8_t
dps_monitor_primitive_var(DpsSlave_h* const restrict self,
        const enum DPS_PRIMITIVE_TYPES type,
        void* const p_data,
        post_update post_update_f,
        const char name[VAR_NAME_LENGTH]);

// INFO: check if a can message is for the dps and if it's the case it executes
// the message
int8_t
dps_check_can_command_recv(DpsSlave_h* const restrict self,
        const struct DpsCanMessage* const mex);

int8_t
dps_slave_destroy(DpsSlave_h* const restrict self);


#ifdef DEBUG
int dps_get_id(const DpsSlave_h* const restrict self);
int dps_print_var(const DpsSlave_h* const restrict self);
#endif // DEBUG

#ifdef __cplusplus
}
#endif

#endif // !__DPS_SLAVE__
