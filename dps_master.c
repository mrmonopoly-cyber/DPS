#include "dps_master.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/mex_types.h"
#include "common/can_mex/board.h"
#include "common/can_mex/info_req.h"
#include "common/can_mex/new_connection.h"
#include "common/messages.h"
#include "lib/c_vector/c_vector.h"

#include <stdlib.h>

//private

#define CHECK_INPUT(IN) if(!IN) return EXIT_FAILURE;

struct dps_master{
    can_send send_f;
    c_vector* boards;
    c_vector* coms;
};

typedef struct {
    uint8_t id;
    char board_name[BOARD_NAME_LENGTH];
    c_vector* vars;
}board_record;

typedef struct{
    uint8_t com_id;
    board_record* board;
}com_record;

static uint8_t new_id(){
    static uint8_t id_generator = 0;
    id_generator++;
    return id_generator-1;
}

static struct dps_master dps;

static int found_board(const void* ele, const void* key){
    const board_record* board = ele;
    const uint8_t* id = key;

    return board->id == *id;
}

static int found_com(const void* ele, const void* key){
    const board_record* board = ele;
    const uint8_t* id = key;

    return board->id == *id;
}

static void dummy_func_const(const void* ele) {}


static void dummy_fun(void* ele){}


//public
int dps_master_init(can_send send_f)
{
    CHECK_INPUT(send_f);

    dps.send_f = send_f;
    {
        struct c_vector_input_init args = {
            .capacity = 10,
            .found_f = found_board,
            .ele_size = sizeof(board_record),
            .free_fun = dummy_fun,
            .print_fun = dummy_func_const,
        };
        dps.boards= c_vector_init(&args);
    }

    if(!dps.boards){ return EXIT_FAILURE;}

    {
        struct c_vector_input_init args = {
            .capacity = 10,
            .found_f = found_com,
            .ele_size = sizeof(com_record),
            .free_fun = dummy_fun,
            .print_fun = dummy_func_const,
        };
        dps.coms = c_vector_init(&args);
    }
    if(!dps.coms){ return EXIT_FAILURE;}

    return EXIT_SUCCESS;
}

int dps_master_new_connection()
{
    new_connection conn = {
    };
    CanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .dps_payload = {
            .mext_type = {NEW_CONNECTION},
            .data = conn.raw_data,
        },
    };

    return dps.send_f(&mex);
}
