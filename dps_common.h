#ifndef __PDS_COMMON__
#define __PDS_COMMON__

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
    uint16_t full_id;
    uint8_t id_block[2];
}can_id;

struct can_message{
    can_id id;
    uint8_t mex_size;
    uint8_t data[CAN_MAX_DATA_SIZE];
};

typedef struct dps_command{
    can_id id_can;
    char name[6];
}dps_command;

#endif // !__PDS_COMMON__
