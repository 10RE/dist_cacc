#include "net_serial.h"

#define INITIAL_DISTANCE 1
#define LEADING_THROTTLE 20

#define SOP '$'
#define EOP '^'

/*
 * serial communication with unity
*/
float vehicle_data[2] = { 0, 0 };
char recieve_buff[128];

void UreadCommand() {
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
          UdecodeCommand(&recieve_buff[0]);
      }
    }
}

void UsendCommand(float data) {
  Serial.write('$');
  Serial.write((char)4);
  data_t d;
  d.f = data;
  for(int i = 0; i < 4; i++) {
    Serial.write(d.c[i]);
  }
  Serial.write('^');
  Serial.flush();
}

void UsendRaw() {
  Serial.print(recieve_buff);
  Serial.flush();
}

void UdecodeCommand(char* command) {
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
    data_t d1;
    data_t d2;
    for (int i = 0; i < 4; i ++) {
      d1.c[i] = command[2 + i];
      d2.c[i] = command[6 + i];
    }
    
    vehicle_data[0] = d1.f;
    vehicle_data[1] = d2.f;
}



// PID control
float computeIdealDistance(float speed) {
  return 1+0.1*speed;
}

int self_id = 0;
bool self_identified = false;

float identify_self(int prev_vehicle_id) {
  if (prev_vehicle_id == 0) {
    self_id = 1;
  }
  else {
    // wait from message from prev vehicle
    // decode message
    // add 1 to message sent by prev_vehicle
  }
}

float throttle;

float leading_speed, leading_throttle;

float actualDistance, idealDistance;
float prevActualDistance, prevIdealDistance;

int Kp = 1;
int Ki = 0;
int Kd = 1;

float p, i, d;  // proportional, integral, derivative

unsigned long breading_time;

NSerial net_serial(115200);

void setup() {
  net_serial.init(115200);
  Serial.begin(115200);

  // initialize PID
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = INITIAL_DISTANCE;
  prevIdealDistance = INITIAL_DISTANCE;
    
  if (self_id == 0) {
    breaking_time = millis() + 30000;
  }
}

void loop() {

  if (self_id == 0) {
    net_serial.broadcastStates(vehicle_data[0], throttle);
  }
  else {
    net_serial.receive(leading_speed, leading_throttle);
  }

  UreadCommand();

  actualDistance = vehicle_data[1];
  idealDistance = computeIdealDistance(leading_speed);

  p = actualDistance - idealDistance;
  i += p;
  d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

  prevActualDistance = actualDistance;
  prevIdealDistance = idealDistance;

  throttle = self_id == 0 ? LEADING_THROTTLE : leading_throttle + Kp * p + Ki * i + Kd * d;
    
  if (self_id == 0 && millis() > breaking_time) {
      if (vehicle_data[0] > 0) {
          throttle = -1;
      } else {
          throttle = 0;
      }
  }
  //Serial.println(throttle);
  //Serial.print("aaaa: ");
  //Serial.println(command);
  UsendCommand(throttle);

  delay(100);
}
