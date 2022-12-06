#include "net_serial.h"

#define INITIAL_DISTANCE 1
#define LEADING_THROTTLE 20

#define LED 13
#define POT A0

#define SOP '$'
#define EOP '^'

int self_id = 1;

bool led_state = LOW;

float throttle;

float leading_speed, leading_throttle;

float actualDistance, idealDistance;
float prevActualDistance, prevIdealDistance;

float Kp = 2;
float Ki = 0.5;
float Kd = 0;

float max_throttle = 1.3;
float max_brake = 6;

float p, i, d;  // proportional, integral, derivative

bool cacc_connection_flag = false;
unsigned long cacc_connection_time_out = 5000;
unsigned long cacc_connection_time_out_start;

unsigned long send_time_start;
unsigned long send_period = 50;
//NSerial net_serial(115200);

/*
 * serial communication with unity
*/
float vehicle_data[2] = { 0, 0 };
char recieve_buff[128];
int recieve_cursor = 0;

void debug_led_init() {
  pinMode(LED, OUTPUT);
}

void update_debug_led_state(int val) {
  digitalWrite(LED, val);
}

void toggle_debug_led() {
  led_state = !led_state;
  digitalWrite(LED, led_state);
}

void UreadCommand() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '$') {
        recieve_buff[0] = c;
        recieve_cursor = 1;
        return;
    }
    if (recieve_cursor > 0 && recieve_cursor < 10) {
      recieve_buff[recieve_cursor] = c;
      recieve_cursor ++;
    }
    else if (recieve_cursor == 10) {
      recieve_buff[recieve_cursor] = c;
      if (c == '^') {
        UdecodeCommand(&recieve_buff[0]);
      }
      recieve_cursor = 0;
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
  // if (command[0] != SOP) {
  //     Serial.println("SOP!");
  //     return;
  //   }
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
    // if (command[10] != EOP) {
    //   // Serial.print("#");
    //   // Serial.print(len);
    //   // for (int i = 0; i < len; i ++) {
    //   //   Serial.print(command[i]);
    //   // }
    //   Serial.print("EOP");
    //   return;
    // }
    data_t d1;
    data_t d2;
    for (int i = 0; i < 4; i ++) {
      d1.c[i] = command[2 + i];
      d2.c[i] = command[6 + i];
    }
    
    vehicle_data[0] = d1.f;
    vehicle_data[1] = d2.f;
}

float get_pot_read() {
  int pot_read = analogRead(POT);
  return pot_read / 1023.0;
}

// PID control
float computeIdealDistance(float speed) {
  return 1+0.1*speed;
}

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

void cacc_connection_time_init() {
  cacc_connection_time_out_start = millis();
}

void send_time_init() {
  send_time_start = millis();
}

void setup() {
  //net_serial.init(115200);
  Serial.begin(115200);

  debug_led_init();

  // initialize PID
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = INITIAL_DISTANCE;
  prevIdealDistance = INITIAL_DISTANCE;

  leading_speed = 0;
  leading_throttle = 0;

  cacc_connection_time_init();
  send_time_init();

}

void PID_update() {

  if (self_id == 0) {
    throttle = (get_pot_read() * (max_throttle + max_brake) - max_brake);
  }
  else {

    actualDistance = vehicle_data[1];
    idealDistance = computeIdealDistance(leading_speed);

    p = actualDistance - idealDistance;
    i += p;
    d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

    prevActualDistance = actualDistance;
    prevIdealDistance = idealDistance;

    if (!cacc_connection_flag) {
      throttle = Kp * p + Ki * i + Kd * d;

      // net_serial.mySerial.println("AT+CIPSEND=12");
      // net_serial.echoFind("OK");
      // net_serial.echoFind(">");  
      // typedef union {
      //   long l;
      //   char c[4];
      // } long_t;
      // long_t temp;
      // temp.l = (long) p;
      // for (int i = 0; i < 4; i++) {
      //   net_serial.mySerial.write(temp.c[i]);
      // }
      // temp.l = (long) vehicle_data[1];
      // for (int i = 0; i < 4; i++) {
      //   net_serial.mySerial.write(temp.c[i]);
      // }
      // temp.l = (long) throttle;
      // for (int i = 0; i < 4; i++) {
      //   net_serial.mySerial.write(temp.c[i]);
      // }
      
      // net_serial.mySerial.println();

    } else {
      throttle = leading_throttle + Kp * p + Ki * i + Kd * d;
    }

    if (throttle > max_throttle) {
      throttle = max_throttle;
    }
    else if (throttle < -max_brake) {
      throttle = -max_brake;
    }
  }
  
}

void loop() {

  update_debug_led_state(cacc_connection_flag);

  if (cacc_connection_time_out_start + cacc_connection_time_out > millis()) {
    cacc_connection_flag = false;
  }

  if (self_id != 0) {
    // receive network msg
    if (net_serial.receive(leading_speed, leading_throttle))  {
      cacc_connection_flag = true;
      cacc_connection_time_out_start = millis();
    }
  }

  PID_update();

  if (self_id == 0) {
    net_serial.broadcastStates(vehicle_data[0], throttle);
  }

  UreadCommand();

  if (self_id == 1) {
    net_serial.broadcastStates(vehicle_data[0], leading_throttle);
  }

  if (send_time_start + send_period < millis()) {
    send_time_start = millis();
    UsendCommand(vehicle_data[1]);
  }
}