#include "unity_serial.h"
#include "Arduino.h"


USerial::USerial(int baudrate) {
  Serial.begin(baudrate);
}

void USerial::floatToString (char* buff, float value) {
    int32_t local_value;
    memcpy(&local_value, &value, 4);
    for (int i = 0; i < 4; i++) {
        buff[i] = (char) (local_value & 0xFF);
        //Serial.print(buff[i]);
        local_value = local_value >> 8;
    }
}

float USerial::stringToFloat (char* s) {
    char buff[4];
    for (int i = 0; i < 4; i ++) {
      buff[i] = s[i];
    }
    float ret = *(float *)&buff;
    return ret;
}

void USerial::readCommand() {
    int id = 0;
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '$') {
          recieve_buff[0] = c;
          id ++;
          while (id < 11) {
            if (Serial.available() > 0) {
              c = Serial.read();
              recieve_buff[id] = c;
              id ++;
            }
          }
          Serial.print(id);
          decodeCommand(&recieve_buff[0]);
      }
    }
}

void USerial::sendCommand(float data) {
    char buff[4];
    char command[7];
    command[0] = '$';
    command[1] = (char) (4); // data length in char
    floatToString(&buff[0], data);
    command[2] = buff[0];
    Serial.print(command[2]);
    command[3] = buff[1];
    command[4] = buff[2];
    command[5] = buff[3];
    command[6] = '^';
    Serial.print(command);
    //decodeCommand(&command[0], 7);
    Serial.flush();
}

void USerial::sendRaw() {
  Serial.print(recieve_buff);
  Serial.flush();
}

void USerial::decodeCommand(char* command) {
  if (command[0] != SOP) {
      Serial.println("SOP!");
      return;
    }
    uint8_t length = (uint8_t) command[1];
    // if (len != (length + 3)) {
    //   Serial.println("#");
    //   Serial.println(len);
    //   Serial.println(length);
    //   Serial.println("length!");
    //   return;
    // }
    //for (int i = 0; i < len; i ++) {
    //  Serial.print(recieve_buff[i]);
    //}
    if (command[10] != EOP) {
      // Serial.print("#");
      // Serial.print(len);
      // for (int i = 0; i < len; i ++) {
      //   Serial.print(command[i]);
      // }
      Serial.print("EOP");
      return;
    }
    
    vehicle_data[0] = stringToFloat(&command[2]);
    vehicle_data[1] = stringToFloat(&command[6]);
    //Serial.println(vehicle_data[0]);
    //Serial.print(vehicle_data[1]);
}

float USerial::getVehicleData(int index) {
  return vehicle_data[index];
}