#include "dps_master.h"
#include "lib/c_vector/c_vector.h"

#include <stddef.h>

typedef struct board_data {
    uint8_t board_id;
    c_vector* vars;
    c_vector* coms;
}board_data;

typedef struct dps {
    c_vector* board_vector;
    can_send send_f;
}dps;

static dps monitor;

static int found_board(const void* l_ele, const void* key){
    return 0;
}

static void dummy_free(void* e){}
static void dummy_print(const void* e){}

#define CHECK_INIT(r) if(!monitor.board_vector || !monitor.send_f) {return r;}

//INFO: init the master
//send_f : function needed to send a can message
void dps_master_init(can_send send_f)
{
    struct c_vector_input_init args = {
        .found_f = found_board,
        .capacity = 8,
        .ele_size = sizeof(board_data),
        .free_fun = dummy_free,
        .print_fun = dummy_print,
    };
    monitor.board_vector = c_vector_init(&args);
}

//INFO: refresh the current set of variable sending a message to all the board 
//in the network
void dps_master_refresh(){
    CHECK_INIT();
}

//INFO: get a vector with all the BOARDS and their VARIABLES and COMMANDS
//DO NOT FREE THE POINTER
const c_vector* dps_master_board_list()
{
    CHECK_INIT(NULL);

    return NULL;
}
 
//INFO: send a mex to a specific board to updated a variable of the board
void dps_master_update(const uint8_t board_id, const uint8_t data_id, const void* value)
{
    CHECK_INIT();
}

//INFO: send a command in the system with a given value
void dps_master_send_command(const can_id id_comm, const void* value)
{
    CHECK_INIT();
}

//INFO: check if a given can message is of the belong to the library
//if it's the case it use it and return 1
//else return 0
uint8_t dps_master_check_can_mex_recv(const can_message* mex)
{
    CHECK_INIT(0);

    return 0;
}
