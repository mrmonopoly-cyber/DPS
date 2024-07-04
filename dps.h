#ifndef __DPS__
#define __DPS__

#include <stdint.h>

typedef uint8_t (*can_send) (uint8_t id_mex, uint8_t* buffer);

typedef struct dps dps;
typedef struct command{
    uint8_t id;
    uint8_t min;
    uint8_t max;
    char description[];
}command;

typedef struct can_message{
    uint8_t id;
    uint8_t mex_size;
    uint8_t buffer[];
}can_message;



#define CAN_ID_DPS 0x131 //INFO: update if needed

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
dps dps_init(can_send send_f);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps *self, void *var, uint32_t size_var);

//INFO: tell to dps a command the board can receive 
void dps_board_command(dps *self, command* comm);

//INFO: check if a can message is for the dps and if it's the case it executes the message
void check_can_command_recv(dps *self, can_message* mex);


#endif // !__DPS__
