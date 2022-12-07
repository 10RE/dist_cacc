#include "net_serial.h"

#define INITIAL_DISTANCE 1
#define LEADING_THROTTLE 20

#define USE_NET_SERIAL 1

#define LED 13
#define POT A0

#define SOP '$'
#define EOP '^'

int self_id = 2;
int vehicle_ahead_id = 1;

bool led_state = LOW;

float throttle;

float vehicle_leading_data[2] = { 0, 0 }; // speed, throttle

float actualDistance, idealDistance;
float prevActualDistance, prevIdealDistance;

float Kp = 1.5;
float Ki = 0;
float Kd = 0;

float max_throttle = 1.3;
float max_brake = 6;

float p, i, d;  // proportional, integral, derivative

bool cacc_connection_flag = false;
unsigned long cacc_connection_time_out = 5000;
unsigned long cacc_connection_time_out_start;

unsigned long send_time_start;
unsigned long send_period = 50;

unsigned long recv_time_start;
unsigned long recv_period = 200;

#if USE_NET_SERIAL
  NSerial net_serial(115200);
  float vehicle_ahead_data[2] = { 0, 0 }; // speed, throttle
#endif

/*
 * serial communication with unity
*/
float vehicle_data[2] = { 0, 0 }; // speed, distance
char recieve_buff[128];
int recieve_cursor = 0;

void debug_led_init() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void update_debug_led_state(int val) {
  digitalWrite(LED, val);
}

void toggle_debug_led() {
  led_state = !led_state;
  digitalWrite(LED, led_state);
}

// void UreadCommand() {
//   if (Serial.available() > 0) {
//     char c = Serial.read();
//     if (c == '$') {
//         recieve_buff[0] = c;
//         recieve_cursor = 1;
//         return;
//     }
//     if (recieve_cursor > 0 && recieve_cursor < 10) {
//       recieve_buff[recieve_cursor] = c;
//       recieve_cursor ++;
//     }
//     else if (recieve_cursor == 10) {
//       recieve_buff[recieve_cursor] = c;
//       if (c == '^') {
//         UdecodeCommand(&recieve_buff[0]);
//       }
//       recieve_cursor = 0;
//     }
//   }
// }

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
          //Serial.print(id);
          if (recieve_buff[10] == '^') {
            UdecodeCommand(&recieve_buff[0]);
          }
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

void reset_pid() {
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = 0;
  prevIdealDistance = 0;
}

void UdecodeCommand(char* command) {
  // if (command[0] != SOP) {
  //     Serial.println("SOP!");
  //     return;
  //   }

    
    uint8_t command_id = (uint8_t) command[1];
    
    if (command_id == 0) {
      
      data_t d1;
      data_t d2;
      for (int i = 0; i < 4; i ++) {
        d1.c[i] = command[2 + i];
        d2.c[i] = command[6 + i];
      }
      
      vehicle_data[0] = d1.f;
      vehicle_data[1] = d2.f;
    }
    else if (command_id == 1) {
      //toggle_debug_led(); 
      reset_pid();
    }

    
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
    
}

float get_pot_read() {
  int pot_read = analogRead(POT);
  return pot_read / 1023.0;
}

// PID control
float computeIdealDistanceACC(float speed) {
  return 3 * (speed / 6 + 1) * (speed / 6 + 1) + 5;
}

float computeIdealDistanceCACC(float speed) {
  float vehicleLen = 15.5;
  float commuLatency = 0.5;
  float lower_safe_boundary_with_comm_latency = 3.65 * commuLatency * commuLatency + vehicleLen * 0.5;
  float ret = 0;
  if (speed < 10) {
    ret = 0.6 * vehicleLen + 0.6 * vehicleLen /(1 + exp(-((speed - 3.5))));
  }
  else {
    ret = 1.2 * vehicleLen + 0.4 * vehicleLen /(1 + exp(-(speed - 13.5)));
  }

  ret = ret < lower_safe_boundary_with_comm_latency ? lower_safe_boundary_with_comm_latency : ret;

  return ret;
}

float prevComputeIdealDistanceCACC(float speed) {
  //return 2.0;
  float d = 6;
  float d_p = 6;
  float vehicleLen = 15.5;
  float commuLatency = 0.5;
  float lower_safe_boundary_with_comm_latency = 3.65 * commuLatency * commuLatency + 3; // 0.5 * (max_acc + abs(max_brake)) * commuLatency^2 + 3;
  float r_safe;
  float boundaryValue1 = 0.3;
  float boundaryValue2 = 1.3;

  float ret = 0;

  //r_safe = speed * speed / 2 * (1 / d - 1 / d_p) + 0.02 * speed;
  r_safe = speed * speed / 2 * (1 / d - 1 / d_p) + 0.1 * speed;

  if (r_safe < boundaryValue1)
    ret = r_safe;
  else if (r_safe <= boundaryValue2)
    ret = 1.2 * vehicleLen;
  else if (r_safe <= 1.6 * vehicleLen)
    ret = 1.6 * vehicleLen;
  else {
    ret = r_safe;
  }

  ret = ret < lower_safe_boundary_with_comm_latency ? lower_safe_boundary_with_comm_latency : ret;

  return ret;
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
  #if USE_NET_SERIAL
    net_serial.init(115200);
  #endif
  Serial.begin(115200);

  debug_led_init();

  // initialize PID
  p = 0;
  i = 0;
  d = 0;
  prevActualDistance = INITIAL_DISTANCE;
  prevIdealDistance = INITIAL_DISTANCE;

  cacc_connection_time_init();
  send_time_init();

}

void PID_update() {

  if (self_id == 0) {
    throttle = (get_pot_read() * (max_throttle + max_brake) - max_brake);
  }
  else {
    if (cacc_connection_flag == false) {
      actualDistance = vehicle_data[1];
      idealDistance = computeIdealDistanceACC(vehicle_data[0]);

      // if (idealDistance < 3) {
      //   idealDistance = 3;
      // }

      p = actualDistance - idealDistance;
      i += p;
      d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

      prevActualDistance = actualDistance;
      prevIdealDistance = idealDistance;

      throttle = Kp * p + Ki * i + Kd * d;
    } else {

      actualDistance = vehicle_data[1];
      //idealDistance = computeIdealDistanceCACC(vehicle_leading_data[0]);
      //if (vehicle_leading_data[1] > ) {
        idealDistance = computeIdealDistanceCACC(vehicle_leading_data[0]);
      // }
      // else if (vehicle_leading_data[1] < -4) {
      //   idealDistance = computeIdealDistanceACC(vehicle_data[0]);
      // }
      

      if (idealDistance < 5) {
        idealDistance = 5;
      }

      p = actualDistance - idealDistance;
      i += p;
      d = (actualDistance - prevActualDistance) - (prevIdealDistance - idealDistance);

      prevActualDistance = actualDistance;
      prevIdealDistance = idealDistance;

      //float safe_brake_distance = 3 * (vehicle_data[0] / 6 + 1) * (vehicle_data[0] / 6 + 1) + 5;

      // if (vehicle_leading_data[1] > 0) {
      //   throttle = vehicle_ahead_data[1] + 0.1 * p + 0.000001 * i +  1 * d;
      // }
      // else {
      //   throttle = vehicle_leading_data[1];
      // }

      if (p < 0) {
        i = 0;
      }
      throttle = vehicle_ahead_data[1] + 0.05 * p + 0.000001 * i + 1 * d;
      // if (vehicle_leading_data[1] > 0) {
      //   throttle = vehicle_ahead_data[1] + 0.05 * p + 0.000001 * i + 1 * d;
      // }
      // else {
        //if (vehicle_data[1] < safe_brake_distance) {
        // if (vehicle_data[0] < 5) {
        //   throttle = Kp * p + Ki * i + Kd * d;
        // }
        // else {
          //throttle = -max_brake + Kp * p + Ki * i + Kd * d;
        // }
        //}
        //else {
        //  throttle = vehicle_ahead_data[1] + 0.05 * p + 0.000001 * i + 2 * d;
        //}
      //}
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

  if (cacc_connection_time_out_start + cacc_connection_time_out < millis()) {
    cacc_connection_flag = false;
  }

#if USE_NET_SERIAL
  if (self_id != 0) {
    if (recv_time_start + recv_period < millis()) {
      // send network msg
      // receive network msg
      uint8_t received_id = -1;
      float received_vehicle_speed = 0;
      float received_vehicle_throttle = 0;
      if (net_serial.receiveStates(received_id, received_vehicle_speed, received_vehicle_throttle))  {
        if (received_id == vehicle_ahead_id) {
          vehicle_ahead_data[0] = received_vehicle_speed;
          vehicle_ahead_data[1] = received_vehicle_throttle;
        }
        if (received_id == 0) {
          vehicle_leading_data[0] = received_vehicle_speed;
          vehicle_leading_data[1] = received_vehicle_throttle;
        }
        cacc_connection_flag = true;
        cacc_connection_time_out_start = millis();
        
      }
    }
  }
#endif

  
  //throttle = vehicle_data[1];

#if USE_NET_SERIAL
  if (recv_time_start + recv_period < millis()) {
    net_serial.broadcastStates(self_id, vehicle_data[0], throttle);
    recv_time_start = millis();
  }
#endif

  UreadCommand();

  if (send_time_start + send_period < millis()) {
    PID_update();
    send_time_start = millis();
    UsendCommand(throttle);
  }
  
}