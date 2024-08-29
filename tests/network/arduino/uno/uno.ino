#include <string.h>
#include <Arduino_CAN.h>

#define DEBUG
#include <dps_slave.h>

uint8_t u8 = 1;
uint16_t u16 = 1;
uint32_t u32 = 1;
int8_t s8 = 1;
int16_t s16 = 1;
int32_t s32 = 1;
float float_v = 1;


static int send(CanMessage *mex) {
    CanMsg msg(CanStandardId(mex->id), mex->dlc, mex->rawMex.raw_buffer);
    char rc = CAN.write(msg);
    if (rc < 0)
    {
        Serial.print("CAN.write(...) failed with error code ");
        Serial.println(rc);
    }

    Serial.println("msg sent id: " + 
            String(msg.id) + 
            " msg data_length: " + 
            String(msg.data_length) + 
            "data: ");
    for(int i =0;i<8;i++){
        Serial.print(msg.data[i]);
        Serial.print(",");
    }
    Serial.println("");
    
    delay(10);
    return 0;
}

void setup() {
  delay(5000);
  Serial.begin(115200);

  if (!CAN.begin(CanBitRate::BR_500k))
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
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &u8;
      var.name[0] = 'u';
      var.name[1] = '8';
      dps_monitor_var_uint8_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &u16;
      var.name[0] = 'u';
      var.name[1] = '1';
      var.name[2] = '16';
      dps_monitor_var_int16_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &u32;
      var.name[0] = 'u';
      var.name[1] = '3';
      var.name[2] = '2';
      dps_monitor_var_uint32_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &s8;
      var.name[0] = 's';
      var.name[1] = '8';
      dps_monitor_var_int8_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &s16;
      var.name[0] = 's';
      var.name[1] = '1';
      var.name[2] = '6';
      dps_monitor_var_int16_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &s32;
      var.name[0] = 's';
      var.name[1] = '3';
      var.name[2] = '2';
      dps_monitor_var_int32_t(&var);
  }
  {
      VariableInfoPrimitiveType var;
      var.var_ptr = &float_v;
      var.name[0] = 'f';
      var.name[1] = 'l';
      var.name[2] = 'o';
      var.name[3] = 'v';
      var.name[4] = '\0';
      dps_monitor_var_float_t(&var);
  }

  CommandInfo com;
  com.name[0] = 'c';
  com.name[1] = '0';
  com.name[2] = '\0';
  com.metadata.full_data.com_id = 12;
  com.metadata.full_data.size = 6;
  com.metadata.full_data.float_num = 1;
  com.metadata.full_data.signe_num = 1;
  if(dps_monitor_command(&com)){
      Serial.println("failed monitor command");
      while(1);
  }

  delay(100);
  dps_print_var();
  Serial.println("init completed");
}

void loop() {
    if (CAN.available())
    {
        CanMsg const msg = CAN.read();
        CanMessage d_mex;
        d_mex.id = msg.id;
        d_mex.dlc = msg.data_length;
        memcpy(d_mex.rawMex.raw_buffer,msg.data,msg.data_length);

        Serial.println("msg received id: " + 
        String(msg.id) + 
        " msg data_length: " + 
        String(msg.data_length) + 
        "data: ");
        for(int i =0;i<8;i++){
            Serial.print(msg.data[i]);
            Serial.print(",");
        }
        Serial.println("");

        if(dps_check_can_command_recv(&d_mex)){
            Serial.println(String("error mex") + String(" dps id ") + String(DPS_CAN_MESSAGE_ID));
        }
    }
}
