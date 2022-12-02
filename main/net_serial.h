#ifndef net_serial_h
#define net_serial_h

#include "Arduino.h"
#include <SoftwareSerial.h>

#include "net_serial.h"

#define TIMEOUT 500 // mS
#define EN 12

class NSerial
{
public:
    SoftwareSerial mySerial; // RX, TX

    typedef union {
        float f;
        char c[sizeof(float)];
    } data_t;

    NSerial(int baudrate);

    void nPinInit();

    void wifiInit();

    void resetTo9600();

    bool echoFind(String keyword);

    bool sendCommand(String cmd, String ack);

    bool sendCommandBare(String cmd);

    void broadcastStates(float speed, float throttle);
    void receive(float & speed, float & throttle);
};

#endif