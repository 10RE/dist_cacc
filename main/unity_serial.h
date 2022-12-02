#ifndef unity_serial_h
#define unity_serial_h

#include "Arduino.h"

class USerial
{
  public:
    USerial(int baudrate);
    void init(int baudrate);
    void sendCommand(float data);
    void readCommand();
    void decodeCommand(char* command);
    void sendRaw();
    float getVehicleData(int index);

    /*
    *  0: speed data
    *  1: distance data
    */
    float vehicle_data[2] = { 0, 0 };
    char recieve_buff[128];

    void floatToString(char* buff, float value);
    float stringToFloat(char* s);
    const char SOP = '$';
    const char EOP = '^';
};

#endif