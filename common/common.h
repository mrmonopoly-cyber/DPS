#ifndef __PDS_COMMON__
#define __PDS_COMMON__

#include "can_mex/board.h"
#include "can_mex/command.h"
#include "can_mex/definitions.h"
#include "can_mex/info_req.h"
#include "can_mex/variable.h"

#include <cstdint>

typedef struct CanMessage{
    uint16_t id;
    uint8_t mex_size;
    union{
        struct{
            BaseMexDps mext_type_dps;
            union{
                //BOARD
                BoardName get_board_name;
                AssignBoardName set_board_name;
                //VAR
                VariableValue current_var;
                VariableModify update_var;
                VariableInfoName info_var_name;
                VariableInfoMetadata info_var_metadata;
                //COMMAND
                CommandInfoName info_com_name;
                CommandInfoMetadata info_com_metadata;
                SendCommandPayload send_command;
                //FUNC
                //TODO: design RPC 
                //RECV_INFO
                ReqInfo req_info;
            };
        };
        RawPayloadCanMex rawMex;
    }payload;
    uint8_t padding_byte;
}CanMessage;

uint8_t t = sizeof(CanMessage);

void init_new_mex(CanMessage* mex, MEX_TYPE dps_type);

#endif // !__PDS_COMMON__
