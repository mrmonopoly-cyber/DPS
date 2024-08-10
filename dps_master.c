#include "dps_master.h"
#include "common/can_mex/base_mex_components/base_payload.h"
#include "common/can_mex/base_mex_components/mex_types.h"
#include "common/can_mex/board.h"
#include "common/can_mex/command.h"
#include "common/can_mex/new_connection.h"
#include "common/can_mex/object.h"
#include "common/messages.h"
#include "lib/c_vector/c_vector.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    CommandInfoMetadata metadata;
    board_record* board;
}com_record;


static uint8_t new_id(){
    static uint8_t id_generator = 0;
    id_generator++;
    return id_generator-1;
}

static struct dps_master dps;
#define CHECK_INIT() if(!dps.send_f || !dps.boards || !dps.coms) return EXIT_FAILURE;

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

static int found_var(const void* ele, const void* key){
    const var_record* var= ele;
    const uint8_t* id = key;

    return var->metadata.full_data.obj_id.data_id == *id;
}

static void dummy_func_const(const void* ele) {}


static void dummy_fun(void* ele){}

static int get_board_name_exec(CanMessage* mex){
    BoardName b_name = {
        .raw_data = mex->dps_payload.data,
    };

    struct c_vector_input_init args = {
        .capacity = 10,
        .ele_size = sizeof(var_record),
        .print_fun = dummy_func_const,
        .free_fun = dummy_fun,
        .found_f = found_var,
    };

    board_record new_board = {
        .id = new_id(),
        .vars = c_vector_init(&args),
    };
    memcpy(new_board.board_name, b_name.full_data.name, BOARD_NAME_LENGTH);

    if (!c_vector_push(&dps.boards, &new_board)){
        return EXIT_FAILURE;
    }

    AssignBoarId board_id = {
        .full_data.obj_id.board_id = new_board.id,
    };
    memcpy(board_id.full_data.name,new_board.board_name,BOARD_NAME_LENGTH);

    CanMessage mex_id = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .dps_payload = {
            .mext_type = {SET_BOARD_ID},
            .data = board_id.raw_data,
        },
    };

    return dps.send_f(&mex_id);
}

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
    CHECK_INIT();

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

int dps_master_check_mex_recv(CanMessage* mex)
{
    CHECK_INIT();

    CHECK_INPUT(mex);

    if(mex->id == DPS_CAN_MESSAGE_ID) {
        switch (mex->dps_payload.mext_type.type) {
            case GET_BOARD_NAME:
                return get_board_name_exec(mex);
        }
    
    }


    return EXIT_FAILURE;
}

//debug

int dps_master_print_boards()
{
    int len = c_vector_length(dps.boards);
    for (int i=0; i<len; i++) {
        board_record* board = c_vector_get_at_index(dps.boards, i);
        if (!board) {
            return EXIT_FAILURE;
        }
        printf("board id: %d,",board->id);
        printf("board name: %s\n",board->board_name);
    }

    return EXIT_SUCCESS;
}


board_list_info* dps_master_list_board()
{
    uint8_t len = c_vector_length(dps.boards);
    board_list_info* res = calloc(1, sizeof(board_list_info) + (sizeof(board_info)* len));
    res->board_num = len;
    for (uint8_t i=0; i<len; i++) {
        board_record* board = c_vector_get_at_index(dps.boards, i);
        if (!board) {
            free(res);
            return NULL;
        }
        res->boards[i].id = board->id;
        res->boards[i].name = board->board_name;
    }
    
    return res;
}
