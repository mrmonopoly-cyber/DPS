#include <string.h>
#include <Arduino_CAN.h>

#include <dps_slave.h>

uint8_t u8 = 1;
uint16_t u16 = 1;
uint32_t u32 = 1;
int8_t s8 = 1;
int16_t s16 = 1;
int32_t s32 = 1;
float float_v = 1;

arduino::R7FA4M1_CAN personal_can(D14, D15);

static int send(CanMessage *mex) {
    CanMsg msg(CanStandardId(mex->id), mex->dlc, mex->rawMex.raw_buffer);
    char rc = personal_can.write(msg);
    if (rc < 0)
    {
        Serial.print("CAN.write(...) failed with error code ");
        Serial.println(rc);
    }
    delay(10);
}

void setup() {
  Serial.begin(115200);

  if (!personal_can.begin(CanBitRate::BR_500k))
  {
      Serial.println("Errore nell'inizializzazione della comunicazione CAN");
      while (1)
          ;
  }
  Serial.println("Comunicazione CAN inizializzata");


  BoardName b_name = {
      "SLAVE",
  };
  if (dps_init(send, &b_name)) {
    Serial.println("init dps failed");
    for (;;) {
    }
  }

#define SAVE_VAR(VAR, fun)                                                     \
  {                                                                            \
    VariableInfoPrimitiveType var;                                             \
    memcpy((void *)var.name, (void *)&VAR, sizeof(VAR));                       \
    var.var_ptr = &VAR;                                                        \
    if (fun(&var)) {                                                           \
      for (;;) {                                                               \
        Serial.println(String("failed monitor") + String(#VAR));               \
      }                                                                        \
    }                                                                          \
  }

  SAVE_VAR(u8, dps_monitor_var_uint8_t);
  SAVE_VAR(u16, dps_monitor_var_uint16_t);
  SAVE_VAR(u32, dps_monitor_var_uint32_t);
  SAVE_VAR(s8, dps_monitor_var_int8_t);
  SAVE_VAR(s16, dps_monitor_var_int16_t);
  SAVE_VAR(s32, dps_monitor_var_int32_t);
  SAVE_VAR(float_v, dps_monitor_var_float_t);
}

void loop() {
    if (personal_can.available())
    {
        CanMsg const msg = personal_can.read();
        CanMessage d_mex;
        d_mex.id = msg.id;
        d_mex.dlc = msg.data_length;
        memcpy(d_mex.rawMex.raw_buffer,msg.data,msg.data_length);

        dps_check_can_command_recv(&d_mex);
    }
}
