#ifndef __DPS_SLAVE__
#define __DPS_SLAVE__

#include <stdint.h>

typedef struct can_message can_message;
typedef uint8_t (*can_send) (can_message* mex);

#define CAN_MAX_DATA_SIZE 8 

enum DPS_ID{
    VARS = 0x131, 
    INFO = 0x132,
    RESP = 0x133,
};

typedef union can_id{
    enum DPS_ID full_id;
    uint8_t id_block[2];
}can_id;

typedef struct dps_command{
    can_id id_can;
    uint8_t min;
    uint8_t max;
    char name[2];
}dps_command;

typedef struct dps_var{
    void* var_ptr;
    uint8_t size;
    char name[5];
}dps_var;

struct can_message{
    can_id id;
    uint8_t mex_size;
    uint8_t data[CAN_MAX_DATA_SIZE];
};



//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
void dps_init(can_send send_f, uint8_t board_id);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var);

//INFO: tell to dps a dps_command the board can receive 
void dps_monitor_command(dps_command* comm);

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t check_can_command_recv(can_message* mex);

#endif // !__DPS_SLAVE__
