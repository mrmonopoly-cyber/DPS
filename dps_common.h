#ifndef __PDS_COMMON__
#define __PDS_COMMON__

#include <stdint.h>

typedef struct can_message can_message;
typedef uint8_t (*can_send) (can_message* mex);

#define CAN_MAX_DATA_SIZE 8 
#define BOARD_NAME_SIZE 6
#define VAR_NAME_SIZE 5
#define COM_NAME_SIZE 4

enum MEX_TYPE{
    VAR,
    COM,
    BRD,
};

enum DPS_ID{
    VARS = 0x131,
    INFO = 0x132,
    RESP = 0x133,
};

typedef union can_id{
    uint16_t full_id;
    uint8_t id_block[2];
}can_id;

struct board_info_slave{
    uint8_t board_id;
    uint8_t mex_type;
    char name[BOARD_NAME_SIZE];
};

struct var_info_slave{
    uint8_t board_id;
    uint8_t mex_type;
    uint8_t id_data;
    char name[VAR_NAME_SIZE];
};

struct com_info_slave{
    uint8_t board_id;
    uint8_t mex_type;
    uint16_t id_can_com;
    char name[COM_NAME_SIZE];
};

struct var_update_master{
    uint8_t board_id;
    uint8_t id_data;
    uint8_t value[6];
};

struct can_message{
    can_id id;
    uint8_t mex_size;
    union{
        uint8_t data[CAN_MAX_DATA_SIZE];
        struct board_info_slave board_slave;
        struct var_info_slave var_slave;
        struct com_info_slave com_slave;
        struct var_update_master upd_master;
    };
};

typedef struct dps_command{
    can_id id_can;
    char name[COM_NAME_SIZE];
}dps_command;

#endif // !__PDS_COMMON__
