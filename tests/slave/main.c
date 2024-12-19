#include <string.h>

#define DEBUG
#include "../../common/can_mex/info_req.h"
#include "../../common/can_mex/new_connection.h"
#include "../../common/can_mex/variable.h"
#include "../../dps_slave.h"

#include <stdint.h>

#include "../test_lib.h"

#define BOARD_ID 5

uint8_t u8 = 0;
uint16_t u16 = 0;
uint32_t u32 = 15;
uint64_t u64 = 0;

int8_t s8 = 0;
int16_t s16 = 0;
int32_t s32 = 0;
int64_t s64 = 0;

float fdata = 0;

uint8_t u8_1 = 0;
uint16_t u16_1 = 0;
uint32_t u32_1 = 15;
uint64_t u64_1 = 0;

int8_t s8_1 = 0;
int16_t s16_1 = 0;
int32_t s32_1 = 0;
int64_t s64_1 = 0;

float fdata_1 = 0;

uint8_t u8_special = 17;
uint8_t u8_post = 0;

void u8_post_update(const char* name, const void* args){
    memcpy(&u8_post, args, sizeof(u8_post));
}

typedef struct {
  uint8_t a;
  uint16_t b;
  uint8_t c;
} generic_type;

generic_type gen_t = {};

int check_monitor_var() {
#define monitor_var(n, fun, macro_ptr)                                         \
  {                                                                            \
    VariableInfoPrimitiveType var_info = {.name = #n, .var_ptr = macro_ptr };  \
    if (fun(&var_info)) {                                                      \
      FAILED("error saving var " #n);                                          \
      return -2;                                                               \
    }                                                                          \
    PASSED("check ok var " #n);                                                \
  }

  monitor_var(u8, dps_monitor_var_uint8_t, &u8);
  monitor_var(u16, dps_monitor_var_uint16_t, &u16);
  monitor_var(u32, dps_monitor_var_uint32_t, &u32);

  monitor_var(s8, dps_monitor_var_int8_t, &s8);
  monitor_var(s16, dps_monitor_var_int16_t, &s16);
  monitor_var(s32, dps_monitor_var_int32_t, &s32);

  monitor_var(fdata, dps_monitor_var_float_t, &fdata);

  monitor_var(u8_1, dps_monitor_var_uint8_t, &u8_1);
  monitor_var(u16_1, dps_monitor_var_uint16_t, &u16_1);
  monitor_var(u32_1, dps_monitor_var_uint32_t, &u32_1);

  monitor_var(s8_1, dps_monitor_var_int8_t, &s8_1);
  monitor_var(s16_1, dps_monitor_var_int16_t, &s16_1);
  monitor_var(s32_1, dps_monitor_var_int32_t, &s32_1);

  monitor_var(fdat_1, dps_monitor_var_float_t, &fdata_1);

  VariableInfoGericType gen = {
      .name = "gen_t",
      .var_ptr = &gen_t,
      .size = sizeof(gen_t),
      .float_var = 0,
      .signd_var = 1,
  };

  if (dps_monitor_var_general(&gen)) {
    FAILED("error saving var gen_t");
    return -2;
  }
  PASSED("check ok save var gen_t");

  VariableInfoPrimitiveType new_var = {
      .post_update_f = u8_post_update,
      .var_ptr = &u8_special,
      .name = "u8_s",
  };
  if(dps_monitor_var_uint8_t(&new_var)){
      FAILED("monitor error special u8");
      return -2;
  }
  PASSED("monitor error special u8");

  return 0;
}

int check_monitor_com() {
#define monitor_com(ID, DLC, NAME, FLOAT, SIGN)                                \
  {                                                                            \
    CommandInfo com = {                                                        \
        .name = NAME,                                                          \
        .metadata.full_data =                                                  \
            {                                                                  \
                .com_id = ID,                                                  \
                .size = DLC,                                                   \
                .float_num = FLOAT,                                            \
                .signe_num = SIGN,                                             \
                .board_id = BOARD_ID,                                          \
            },                                                                 \
    };                                                                         \
    if (dps_monitor_command(&com)) {                                           \
      FAILED("monitor command save failed");                                   \
      return -1;                                                               \
    }                                                                          \
    PASSED("monitor command save success");                                    \
  }

  monitor_com(15, 6, "CM_1S", 0, 0);
  monitor_com(20, 8, "CM_2S", 1, 1);

  return 0;
}

int check_update_var() {
#define update_var(VID, VALUE_T, VALUE_D)                                      \
  {                                                                            \
    VALUE_T mod = VALUE_D;                                                     \
    VariableModify var = {                                                     \
        .full_data.obj_id.data_id = VID,                                       \
        .full_data.obj_id.board_id = BOARD_ID,                                 \
    };                                                                         \
    memcpy(var.full_data.value, &mod, sizeof(mod));                            \
    DPSCanMessage mex = {                                                         \
        .id = DPS_CAN_MESSAGE_ID,                                              \
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,                                  \
        .GenericPayload.dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},       \
        .GenericPayload.dps_payload.data = var.raw_data,                       \
    };                                                                         \
    if (dps_check_can_command_recv(&mex)) {                                    \
      FAILED("update mex not recognized");                                     \
      return -1;                                                               \
    }                                                                          \
    PASSED("update mex recognized");                                           \
  }

  update_var(0, uint8_t, 1);
  update_var(1, uint16_t, 2);
  update_var(2, uint32_t, 3);

  update_var(3, int8_t, -1);
  update_var(4, int16_t, -2);
  update_var(5, int32_t, -3);

  update_var(15, uint8_t, 29);

  {
    float new_value = 2.5f;
    VariableModify var = {
        .full_data.obj_id.data_id = 6,
        .full_data.obj_id.board_id = BOARD_ID,
    };
    mempcpy(var.full_data.value, &new_value, sizeof(new_value));
    DPSCanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .GenericPayload.dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},
        .GenericPayload.dps_payload.data = var.raw_data,
    };
    if (dps_check_can_command_recv(&mex)) {
      FAILED("update mex not recognized");
      return -1;
    }
    PASSED("update mex recognized");
  }

  {
    generic_type new_value = {
        .a = 4,
        .b = 12,
        .c = 24,
    };
    VariableModify var = {
        .full_data.obj_id.data_id = 14,
        .full_data.obj_id.board_id = BOARD_ID,
    };
    mempcpy(var.full_data.value, &new_value, sizeof(new_value));
    DPSCanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .GenericPayload.dps_payload.mext_type = {SET_CURRENT_VAR_VALUE},
        .GenericPayload.dps_payload.data = var.raw_data,
    };
    if (dps_check_can_command_recv(&mex)) {
      FAILED("update mex not recognized");
      return -1;
    }
    PASSED("update mex recognized");
  }

  u8 == 1 ? PASSED("u8 updated correctly") : FAILED("u8 update failed");
  u16 == 2 ? PASSED("u16 updated correctly") : FAILED("u16 update failed");
  u32 == 3 ? PASSED("u32 updated correctly") : FAILED("u32 update failed");

  s8 == -1 ? PASSED("s8 updated correctly") : FAILED("s8 update failed");
  s16 == -2 ? PASSED("s16 updated correctly") : FAILED("s16 update failed");
  s32 == -3 ? PASSED("s32 updated correctly") : FAILED("s32 update failed");

  fdata == 2.5f ? PASSED("fdata updated correctly")
                : FAILED("fdata update failed");

  (u8_special == 29 && u8_post == 29) ? PASSED("u8_special updated correctly") : FAILED("u8_special update failed");

  if (gen_t.a != 4 || gen_t.b != 12 || gen_t.c != 24) {
    FAILED("generic type update failed");
    return -1;
  }
  PASSED("generic type update ok");


  return 0;
}

int send_com_info_master() {
  ReqInfo req = {
      .full_data.data_it.board_id = BOARD_ID,
      .full_data.info_t = COMMAND,
  };

  DPSCanMessage com_info_req = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .GenericPayload.dps_payload =
          {
              .mext_type = {GET_INFO},
              .data = req.raw_data,
          },
  };

  if (dps_check_can_command_recv(&com_info_req)) {
    FAILED("info req command not recognized");
    return -1;
  }
  PASSED("info req command recognized");

  return 0;
}

int send_var_info_master() {
  ReqInfo req = {
      .full_data.data_it.board_id = BOARD_ID,
      .full_data.info_t = VAR,
  };

  DPSCanMessage com_info_req = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .GenericPayload.dps_payload =
          {
              .mext_type = {GET_INFO},
              .data = req.raw_data,
          },
  };

  if (dps_check_can_command_recv(&com_info_req)) {
    FAILED("info req variables not recognized");
    return -1;
  }
  PASSED("info req variables recognized");

  return 0;
}

int get_cur_values() {
  ReqInfo req_val_wrong = {
      .full_data =
          {
              .info_t = VAR_VALUE,
              .data_it.board_id = 7,
              .data_it.data_id = 2,
          },
  };

  ReqInfo req_val_right = {
      .full_data =
          {
              .info_t = VAR_VALUE,
              .data_it.board_id = BOARD_ID,
              .data_it.data_id = 2,
          },
  };

  DPSCanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .GenericPayload.dps_payload.mext_type = {GET_INFO},
      .GenericPayload.dps_payload.data = req_val_wrong.raw_data,
  };

  if (!dps_check_can_command_recv(&mex)) {
    FAILED("wrong variable accessed");
    return -1;
  }
  PASSED("wrong variable read refused");

  mex.GenericPayload.dps_payload.data = req_val_right.raw_data;
  if (dps_check_can_command_recv(&mex)) {
    FAILED("correct variable read failed");
    return -1;
  }
  PASSED("correct variable read ok");

  return 0;
}

int check_link_req() {
  new_connection conn = {};
  DPSCanMessage mex = {
      .id = DPS_CAN_MESSAGE_ID,
      .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
      .GenericPayload.dps_payload.mext_type = {NEW_CONNECTION},
      .GenericPayload.dps_payload.data = conn.raw_data,
  };

  if (dps_check_can_command_recv(&mex)) {
    FAILED("new connection message not recognized");
    return -1;
  }
  PASSED("new connection message recognized");
  return 0;
}

int check_id_assign() {
  AssignBoarId correct_assign = {
      .full_data.obj_id.board_id = BOARD_ID,
      .full_data.name = "TSLAVE",
  };

  AssignBoarId wrong_assign = {
      .full_data.obj_id.board_id = 2,
      .full_data.name = "WRONG",
  };
  {
    DPSCanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .GenericPayload.dps_payload.mext_type = {SET_BOARD_ID},
        .GenericPayload.dps_payload.data = correct_assign.raw_data,
    };
    if (dps_check_can_command_recv(&mex)) {
      FAILED("can message for id assignment not recognized");
      return -1;
    }
    PASSED("can message for id assignment correct recognized");
    if (dps_get_id() != correct_assign.full_data.obj_id.board_id) {
      FAILED("correct id to board not assigned");
      return -2;
    }
    PASSED("correct id to board assigned");
  }

  {
    DPSCanMessage mex = {
        .id = DPS_CAN_MESSAGE_ID,
        .dlc = CAN_PROTOCOL_MAX_PAYLOAD_SIZE,
        .GenericPayload.dps_payload.mext_type = {SET_BOARD_ID},
        .GenericPayload.dps_payload.data = wrong_assign.raw_data,
    };
    if (dps_check_can_command_recv(&mex)) {
      FAILED("can message for id assignment not recognized");
      return -1;
    }
    PASSED("can message for id assignment wrong recognized");
    if (dps_get_id() == wrong_assign.full_data.obj_id.board_id) {
      FAILED("wrong id to board assigned");
      return -2;
    }
    PASSED("wrong id to board not assigned");
  }
  return 0;
}

int send(DPSCanMessage *mex) { return 0; }

int run_test() {
  BoardName board_name = {
      .full_data.name = "TSLAVE",
  };

  if (dps_init(send, &board_name)) {
    FAILED("failed init of slave");
    return -1;
  }
  PASSED("init completed");

  if (dps_slave_start()) {
    FAILED("failed dps not started");
    return -2;
  }
  PASSED("passed dps started");

  if (check_link_req()) {
    FAILED("failed to receive link request");
    return -2;
  }

  if (check_id_assign()) {
    FAILED("failed assign id board");
    return -3;
  }

  if (check_monitor_var()) {
    FAILED("failed saving vars");
    return -4;
  }

  if (dps_slave_disable()) {
    FAILED("failed dsp disable");
    return -1;
  }
  PASSED("passed dsp disable");

  if (dps_slave_start()) {
    FAILED("failed dps start");
    return -1;
  }
  PASSED("passed dsp start");

  if (check_monitor_com()) {
    FAILED("failed saving coms");
    return -5;
  }

  if (check_update_var()) {
    FAILED("failed updating vars");
    return -6;
    ;
  }

  if (send_com_info_master()) {
    FAILED("failed send info com master");
    return -7;
    ;
  }
  PASSED("send info com master ok");

  if (send_var_info_master()) {
    FAILED("failed send info var master");
    return -7;
    ;
  }
  PASSED("send info var master ok");

  if (get_cur_values()) {
    FAILED("failed sending current var values");
    return -7;
    ;
  }
  PASSED("sending current var values ok");

  return 0;
}

int main(void) {
  run_test();

  dps_print_var();
  print_SCORE();
  return 0;
}
