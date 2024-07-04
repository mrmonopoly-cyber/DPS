#ifndef __DPS__
#define __DPS__

#include <stdint.h>

typedef uint8_t (*can_send) (uint8_t id_mex, uint8_t* buffer);

typedef struct dps dps;
typedef struct dps_command{
    uint8_t id;
    uint8_t min;
    uint8_t max;
    char description[];
}dps_command;

typedef struct dps_var{
    void* var_ptr;
    uint32_t size;
    char description[];
}dps_var;

typedef struct can_message{
    uint8_t id;
    uint8_t mex_size;
    uint8_t buffer[];
}can_message;



#define CAN_ID_DPS 0x131 //INFO: update if needed

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
dps* dps_init(can_send send_f);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps *self, void *dps_var, uint32_t size_var);

//INFO: tell to dps a dps_command the board can receive 
void dps_board_command(dps *self, dps_command* comm);

//INFO: check if a can message is for the dps and if it's the case it executes the message
void check_can_command_recv(dps *self, can_message* mex);

void dps_free(dps *self);

#endif // !__DPS__
