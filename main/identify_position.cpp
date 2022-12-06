/*
int self_id;
int cars_ahead;

typedef union {
  float f;
  char c[sizeof(float)];
} float_t;

typedef union {
  long i;
  char c[sizeof(long)];
} int_t;

// package protocol
// speed and throttle: "ST + speed + throttle"
// vehicle order info: "OR + id + No. cars ahead"

void broadcastStates(float speed, float throttle) {
    float_t s, t;
    s.f = speed;
    t.f = throttle;
    mySerial.println("AT+CIPSEND=10");
    echoFind("OK");
    echoFind(">"); 
    mySerial.print("ST");
    for (int i = 0; i < 4; i++) {
      mySerial.write(s.c[i]);
    }
    for (int i = 0; i < 4; i++) {
      mySerial.write(t.c[i]);
    }
    mySerial.println();
}

void broadcastOrder() {
    int_t id, cars;
    id.i = self_id;
    cars.i = cars_ahead;
    mySerial.println("AT+CIPSEND=10");
    echoFind("OK");
    echoFind(">"); 
    mySerial.print("OR");
    for (int i = 0; i < 4; i++) {
      mySerial.write(id.c[i]);
    }
    for (int i = 0; i < 4; i++) {
      mySerial.write(cars.c[i]);
    }
    mySerial.println();
}

void updateST(float & speed, float & throttle) {

}

void updatePosition(int & position) {
    int_t p;
    for (int i = 0; i < sizeof(int); i++) {
        mySerial.readBytes(p.c, sizeof(int));
    }
}



bool NSerial::receive(float & speed, float & throttle) {
    float_t s, t;
    if (echoFind("+IPD,:")) {
        for (int i = 0; i < 4; i++) {
            while (!mySerial.available()) {}
            s.c[i] = mySerial.read();
        }
        for (int i = 0; i < 4; i++) {
            while (!mySerial.available()) {}
            t.c[i] = mySerial.read();
        }
        speed = s.f;
        throttle = t.f;
        return true;
    }
    return false;
}

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





    if (echoFind("+IPD,10:")) {
      char headerBuffer[2];
      mySerial.readBytes(headerBuffer,2)
      // if msg type is ST
      if ()
      // if msg type is OR

    }

    */



   
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