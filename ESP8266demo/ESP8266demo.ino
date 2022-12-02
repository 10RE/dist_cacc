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
  delay(100);
  SendCommand("AT+RST", "ready");
  delay(100);
  SendCommand("AT+UART_DEF=9600,8,1,0,0", "ready");
  delay(100);

  mySerial.end();
  delay(100);
  mySerial.begin(9600);
  delay(100);

  SendCommand("AT+GMR", "OK");
  delay(100);
  SendCommand("AT+CWMODE=3","OK");
  SendCommand("AT+CWSAP=\"ESP\",\"testtest\",11,4,3", "OK");
  delay(100);
  SendCommand("AT+CIPMUX=1", "OK");
  SendCommand("AT+CIPAP=\"192.168.5.1\"", "OK");
  SendCommand("AT+CIPSERVER=1", "OK");
  delay(100);
  SendCommand("AT+CIPAP?", "");
  delay(100);
  SendCommand("AT+CIFSR", "OK");
  delay(100);
  SendCommand("AT+CIPDINFO=1", "OK");
  delay(100);
  
  //SendCommand("AT+CIPSTART=1,\"TCP\",\"192.168.5.2\",80", "");
  //delay(100);
}

void loop()
{
  //SendCommandBare("AT+CWMODE?");
  SendCommand("AT+CIPSTATUS", "OK");
  delay(100);
  // String page = "test";
  // String cip_send = "AT+CIPSEND=0,4";
  // String cip_end = "AT+CIPCLOSE=0";
  // SendCommand(cip_send, "OK");
  // delay(100);
  // SendCommand(page, "OK");
  // delay(100);
  // SendCommand(cip_end, "OK");
  // delay(100);
  if (millis() > next_cycle) {
    
    // String page = "test";
    // Serial.println(page.length());
    //String cip_send = "AT+CIPSEND=1,4";
    //String cip_end = "AT+CIPCLOSE=1";
    //SendCommand(cip_send, "OK");
    // SendCommand(page, "OK");;
    //delay(100);
    //next_cycle = millis() + cycle_time;
    //delay(1000);
    // SendCommand(cip_end, "OK");
    // delay(100);
  }
  if (mySerial.available()){
    char ch = mySerial.read();
    Serial.write(ch);
  }
}