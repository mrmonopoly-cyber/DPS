#include "dps.h"

#include <stddef.h>
#include <stdlib.h>

typedef struct dps_vector{
    void* buffer;
    uint16_t num_elements;
}dps_vector;

struct dps{
    dps_vector commands;
    dps_vector vars;
};

//private

dps_vector dps_vector_new(){
    return dps_vector{
        .buffer = NULL,
        .num_elements = 0,       
    };
}


//public

//INFO: create dps manager
//send_f : function to send through CAN the data to the external controller
dps* dps_init(can_send send_f)
{
    dps* res = (dps *) malloc(sizeof(*res));
    res->vars = dps_vector_new();
    res->commands = dps_vector_new();

   return res;
}

//INFO: tell to dps to monitor a variable
void dps_monitor_var(dps *self, void *var, uint32_t size_var);

//INFO: tell to dps a command the board can receive 
void dps_board_command(dps *self, dps_command* comm);


//INFO: check if a can message is for the dps and if it's the case it executes the message
void check_can_command_recv(dps *self, can_message* mex);

void dps_free(dps *self);
