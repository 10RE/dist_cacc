#define SOP '$'
#define EOP '^'

/*
*  0: speed data
*  1: distance data
*/
float vehicle_data[2] = {0, 0};
String recieve_buff = "";

String floatToString (float value) {
    String ret = "";
    int32_t local_value;
    memcpy(&local_value, &value, 8);
    for (int i = 0; i < 4; i++) {
        ret += (char) (local_value & 0xFF);
        local_value >>= 8;
    }
    return ret;
}

float stringToFloat (String s) {
    char buff[4];
    for (int i = 0; i < 4; i ++) {
      buff[i] = s[i];
    }
    float ret = *(float *)&buff;
    return ret;
}

String formCommand(float data) {
    String command = "$";
    command += (char) (4); // data length in char
    command += floatToString(data);
    command += "^";
    return command;
}

void sendCommand(String command) {
    Serial.println(command);
    Serial.flush();
}

void decodeCommand(String command) {
    if (command[0] != SOP) {
      return;
    }
    uint8_t length = (uint8_t) command[1];
    if (command.length() < (length + 3)) {
      return;
    }
    if (command[length + 2] != EOP) {
      return;
    }
    vehicle_data[0] = stringToFloat(command.substring(2, 6));
    vehicle_data[1] = stringToFloat(command.substring(6, 10));
    Serial.println(vehicle_data[0]);
    Serial.println(vehicle_data[1]);
}

void setup() {
    Serial.begin(115200);
}

void loop() {

    while (Serial.available()) {
        if (Serial.available() > 0) {
          char c = Serial.read();  //gets one byte from serial buffer
          recieve_buff += c; //makes the string readString
        } 
    }
    if (recieve_buff.length() > 0) {
        decodeCommand(recieve_buff);
        recieve_buff = "";
    }

    String command = formCommand(0.8);
    sendCommand(command);
    
    delay(1000);
}