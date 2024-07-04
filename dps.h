#ifndef __DPS__
#define __DPS__

#include <stdint.h>

typedef struct can_message can_message;
typedef uint8_t (*can_send) (can_message* mex);

#define CAN_MAX_DATA_SIZE 8 //INFO:MAX SIZE CAN MESSAGE DATA
#define ID_TYPE uint8_t
#define NAME_VAR_BUFF_SIZE CAN_MAX_DATA_SIZE - sizeof(uint8_t) - sizeof(ID_TYPE)
#define NAME_COM_BUFF_SIZE CAN_MAX_DATA_SIZE - 2*sizeof(uint8_t) - sizeof(uint16_t)

typedef struct dps_command{
    uint16_t id_can;
    uint8_t min;
    uint8_t max;
    char name[NAME_COM_BUFF_SIZE];
}dps_command;

typedef struct dps_var{
    ID_TYPE id_data;
    void* var_ptr;
    uint8_t size;
    char name[NAME_VAR_BUFF_SIZE];
}dps_var;

struct can_message{
    uint16_t id;
    uint8_t mex_size;
    uint8_t data[CAN_MAX_DATA_SIZE];
};


enum DPS_ID{
    VARS = 0x131, //INFO: change if need
    INFO = 0x132,//INFO: change if need
};

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
void dps_init(can_send send_f);

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps_var* var);

//INFO: tell to dps a dps_command the board can receive 
void dps_board_command(dps_command* comm);

//INFO: check if a can message is for the dps and if it's the case it executes the message
uint8_t check_can_command_recv(can_message* mex);

#endif // !__DPS__
