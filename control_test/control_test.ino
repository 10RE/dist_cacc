#define SOP '$'
#define EOP '^'
#define INITIAL_DISTANCE 1

/*
*  0: speed data
*  1: distance data
*/
float vehicle_data[2] = { 0, 0 };
String recieve_buff = "";

String floatToString(int value) {
  String ret = "";
  int32_t local_value;
  memcpy(&local_value, &value, 8);
  for (int i = 0; i < 4; i++) {
    ret += String((char)(local_value & 0xFF));
    local_value >>= 8;
  }
  return ret;
}

float stringToFloat(String s) {
  char buff[4];
  for (int i = 0; i < 4; i++) {
    buff[i] = s[i];
  }
  float ret = *(float *)&buff;
  return ret;
}

String formCommand(float data) {
  String command = "$";
  command += (char)(4);  // data length in char
  command += floatToString(data);
  Serial.print("command : ");
  Serial.println(command);
  command += "^";
  //Serial.print("before return: ");
  //Serial.println(command);
  return command;
}

void sendCommand(float data) {
    char buff[4];
    char command[7];
    command[0] = '$';
    command[1] = (char) (4); // data length in char
    floatToString(&buff[0], data);
    command[2] = buff[0];
    command[3] = buff[1];
    command[4] = buff[2];
    command[5] = buff[3];
    command[6] = '^';
    Serial.print(command);
    Serial.flush();
}

void decodeCommand(String command) {
  if (command[0] != SOP) {
    return;
  }
  uint8_t length = (uint8_t)command[1];
  if (command.length() < (length + 3)) {
    return;
  }
  if (command[length + 2] != EOP) {
    return;
  }
  vehicle_data[0] = stringToFloat(command.substring(2, 6));
  vehicle_data[1] = stringToFloat(command.substring(6, 10));
  //Serial.println(vehicle_data[0]);
  //Serial.println(vehicle_data[1]);
}

// PID control
float computeIdealDistance(float speed) {
  return 1;
}

float throttle;

float actualDistance, idealDistance;
float prevActualDistance, prevIdealDistance;

int Kp = 1;
int Ki = 0;
int Kd = 1;

float p, i, d;  // proportional, integral, derivative

void setup() {
  Serial.begin(115200);

  // initialize PID
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = INITIAL_DISTANCE;
  prevIdealDistance = INITIAL_DISTANCE;
}

void loop() {

  while (Serial.available()) {
    if (Serial.available() > 0) {
      char c = Serial.read();  //gets one byte from serial buffer
      recieve_buff += c;       //makes the string readString
    }
  }
  if (recieve_buff.length() > 0) {
    decodeCommand(recieve_buff);
    recieve_buff = "";
  }

  actualDistance = vehicle_data[1];
  idealDistance = computeIdealDistance(vehicle_data[0]);

  p = actualDistance - idealDistance;
  i += p;
  d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

  prevActualDistance = actualDistance;
  prevIdealDistance = idealDistance;

  throttle = Kp * p + Ki * i + Kd * d;
  //Serial.println(throttle);
  String command = formCommand(10.0);
  //Serial.print("aaaa: ");
  Serial.println(command);
  //sendCommand(command);

  delay(1000);
}