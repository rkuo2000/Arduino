// ESP32 SPI to PMW3901 optical flow sensor
// SS  : GIOP5
// SCK : GIOP18
// MOSI: GIOP23
// MISO: GIOP19

#include "Bitcraze_PMW3901.h"

Bitcraze_PMW3901 flow(5);

void setup() {
  Serial.begin(115200);

  if (!flow.begin()) {
    Serial.println("Initialization of the flow sensor failed");
    while(1) { }
  }
}

int16_t deltaX,deltaY;

void loop() {
  // Get motion count since last call
  flow.readMotionCount(&deltaX, &deltaY);

  Serial.print("X: ");
  Serial.print(deltaX);
  Serial.print(", Y: ");
  Serial.print(deltaY);
  Serial.print("\n");

  delay(100);
}
