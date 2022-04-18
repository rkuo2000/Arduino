#include <SoftwareSerial.h>
// NodeMCU pins
#define RX_PIN D4 // connect to TXD of module
#define TX_PIN D3 // connect to RXD of module (logic level 3.3v!)

SoftwareSerial swSer(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);
  swSer.begin(115200);
}

void loop() {
  int i;
  while (swSer.available() > 0) {
    Serial.write(swSer.read());
  }
  while (Serial.available() > 0) {
    swSer.write(Serial.read());
  }
}
