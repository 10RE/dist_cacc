#include "net_serial.h"

#define TIMEOUT 500 // mS
#define EN 12

NSerial::NSerial(int baudrate) : mySerial(7, 6) {

}

void NSerial::init(int baudrate) {
    nPinInit();
    mySerial.begin(baudrate);
    delay(100);
    resetTo9600();
    wifiInit();
}

void NSerial::nPinInit() {
    pinMode(EN, OUTPUT);
    digitalWrite(EN, 1);
    //mySerial = SoftwareSerial::SoftwareSerial();
}

void NSerial::wifiInit() {
    // connect to WIFI
    sendCommandBare("AT+CWJAP=\"ESP123\",\"password\"");
    while (!echoFind("WIFI GOT IP")) {
    }
    while (!echoFind("OK")) {
        sendCommandBare("AT");
    }

    // listen for UDP broadcast
    mySerial.println("AT+CIPSTART=\"UDP\",\"192.168.217.255\",15840,15840,0");
    delay(1000);
}

void NSerial::resetTo9600() {
    sendCommandBare("AT+RST");
    delay(100);
    sendCommandBare("AT+UART_DEF=9600,8,1,0,0");
    delay(100);
    mySerial.end();
    delay(100);
    mySerial.begin(9600);
    delay(100);
}

bool NSerial::echoFind(String keyword) {
    int current_char = 0;
    int keyword_length = keyword.length();
    long deadline = millis() + TIMEOUT;
    while(millis() < deadline){
        if (mySerial.available()){
            char ch = mySerial.read();
            //Serial.write(ch);
            if (ch == keyword[current_char]) {
                if (current_char == keyword_length-1) {
                    //Serial.println();
                    return true;
                }
                current_char ++;
            }
        }
    }
    return false; // Timed out
}

bool NSerial::sendCommand(String cmd, String ack){
mySerial.println(cmd); // Send "AT+" command to module
return echoFind(ack);
}

bool NSerial::sendCommandBare(String cmd){
mySerial.println(cmd); // Send "AT+" command to module
}

void NSerial::broadcastStates(float speed, float throttle) {
    data_t s, t;
    s.f = speed;
    t.f = throttle;
    mySerial.println("AT+CIPSEND=8");
    echoFind("OK");
    echoFind(">");  
    for (int i = 0; i < 4; i++) {
    mySerial.write(s.c[i]);
    }
    for (int i = 0; i < 4; i++) {
    mySerial.write(t.c[i]);
    }
    mySerial.println();
}

bool NSerial::receive(float & speed, float & throttle) {
    data_t s, t;
    if (echoFind("+IPD,8:")) {
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