#include <SoftwareSerial.h>
#define TIMEOUT 500 // mS
#define EN 12
#define LED 13

SoftwareSerial mySerial(7, 6); // RX, TX
int cycle_time = 2000;
int next_cycle = 0;

bool echoFind(String keyword) {
  int current_char = 0;
  int keyword_length = keyword.length();
  long deadline = millis() + TIMEOUT;
  while(millis() < deadline){
    if (mySerial.available()){
      char ch = mySerial.read();
      delay(1);
      Serial.write(ch);
      if (ch == keyword[current_char]) {
        if (current_char == keyword_length) {
          Serial.println();
          return true;
        }
        current_char ++;
      }
    }
  }
  return false; // Timed out
}

bool SendCommand(String cmd, String ack){
  mySerial.println(cmd); // Send "AT+" command to module
  if (!echoFind(ack)) // timed out waiting for ack string
    return true; // ack blank or ack found
}

bool SendCommandBare(String cmd){
  mySerial.println(cmd); // Send "AT+" command to module
}

void setup()
{
  pinMode(EN, OUTPUT);
  digitalWrite(EN, 1);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  mySerial.begin(115200);
  SendCommand("AT+RST", "ready");
  delay(100);
  SendCommand("AT+CWMODE=3","OK");
  SendCommand("AT+CWSAP=\"ESP\",\"testtest\",1,4", "OK");
  delay(100);
  SendCommand("AT+CIPMUX=1", "OK");
  SendCommand("AT+CIPAP=\"192.168.5.1\"", "OK");
  SendCommand("AT+CIPSERVER=1,80", "OK");
  delay(100);
  SendCommand("AT+CIPAP?", "");
  delay(100);
  SendCommand("AT+CIPSTART=\"TCP\",\"192.168.5.1\",80", "");
  delay(100);
}

void loop()
{
  //SendCommandBare("AT+CWMODE?");
  if (millis() > next_cycle) {
    mySerial.println("AT+CIPSEND=0,23");
    delay(100);
    mySerial.println("<h1>test</h1>");
    delay(100);
    next_cycle = millis() + cycle_time;
  }
  if (mySerial.available()){
      char ch = mySerial.read();
      Serial.write(ch);
    }
}