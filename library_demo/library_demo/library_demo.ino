#include <SoftwareSerial.h>
#include <SerialESP8266wifi.h>

#define sw_serial_rx_pin 7 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 6 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 5 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define EN 12

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// the last parameter sets the local echo option for the ESP8266 module..
SerialESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin, Serial);//adding Serial enabled local echo and wifi debug

String inputString;
boolean stringComplete = false;
unsigned long nextPing = 0;

void setup() {
  pinMode(EN, OUTPUT);
  digitalWrite(EN, 1);

  inputString.reserve(20);
  swSerial.begin(115200);
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting wifi");

  wifi.setTransportToTCP();// this is also default
  //wifi.setTransportToUDP();//Will use UDP when connecting to server, default is TCP
  Serial.println("TCP finished");

  wifi.endSendWithNewline(true); // Will end all transmissions with a newline and carrage return ie println.. default is true
  Serial.println("New line  ");

  bool wifi_started = wifi.begin();
  Serial.println(wifi_started);
  Serial.println("Wifi started");

  //Turn on local ap and server (TCP)
  wifi.startLocalAPAndServer("test", "testtest", "1", "80");

  //wifi.connectToAP("wifissid", "wifipass");
  //wifi.connectToServer("192.168.0.28", "2121");
  //wifi.send(SERVER, "ESP8266 test app started");
  Serial.println("Wifi setup finished");
}

void loop() {

}