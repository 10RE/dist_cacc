#ifndef net_serial_h
#define net_serial_h

#include "Arduino.h"
#include <SoftwareSerial.h>

#include "net_serial.h"

#define TIMEOUT 500 // mS
#define EN 12

typedef union {
    float f;
    char c[sizeof(float)];
} data_t;

class NSerial
{
public:
    SoftwareSerial mySerial; // RX, TX

    NSerial(int baudrate);

    void init(int baudrate);

    void nPinInit();

    void wifiInit();

    void resetTo9600();

    bool echoFind(String keyword);

    bool sendCommand(String cmd, String ack);

    bool sendCommandBare(String cmd);

    void broadcastStates(uint8_t id, float speed, float throttle);
    
    bool receiveStates(uint8_t & id, float & speed, float & throttle);
};

#endif