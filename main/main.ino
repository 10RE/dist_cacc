#include "unity_serial.h"
#include "net_serial.h"

#define INITIAL_DISTANCE 1
#define LEADING_THROTTLE 20

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

USerial unity_serial(115200);
NSerial net_serial(115200);


void setup() {
  net_serial.init(115200);
  unity_serial.init(115200);
  // initialize PID
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = INITIAL_DISTANCE;
  prevIdealDistance = INITIAL_DISTANCE;
}

void loop() {

  if (self_id == 0) {
    net_serial.broadcastStates(unity_serial.getVehicleData(0), throttle);
  }
  else {
    net_serial.receive(leading_speed, leading_throttle);
  }

  unity_serial.readCommand();

  actualDistance = unity_serial.getVehicleData(1);
  idealDistance = computeIdealDistance(leading_speed);

  p = actualDistance - idealDistance;
  i += p;
  d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

  prevActualDistance = actualDistance;
  prevIdealDistance = idealDistance;

  throttle = self_id == 0 ? LEADING_THROTTLE : Kp * p + Ki * i + Kd * d;
  //Serial.println(throttle);
  //Serial.print("aaaa: ");
  //Serial.println(command);
  unity_serial.sendCommand(throttle);

  delay(1000);
}