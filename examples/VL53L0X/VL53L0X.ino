// VL530LX is an Infrared Ranger
// Max. Distance < 2 meter

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: GIOP22)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: GIOP21)

#include <Wire.h>
#include "VL53L0X.h"

VL53L0X ranger;

unsigned long lastTimeRanger;
unsigned long loopDelayRanger = 100; //ms
int distance;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  ranger.init();
  ranger.setTimeout(500);

  // Start continuous back-to-back mode (take readings as fast as possible).  
  // To use continuous timed mode, instead, provide a desired inter-measurement period in ms
  // (e.g. ranger.startContinuous(100))
  //ranger.startContinuous();
  
}

void loop()
{  
  /* Continuouse Read */ 
  // this mode consume much more power
  //Serial.print(ranger.readRangeContinuousMillimeters());
  //Serial.println(" mm");
  //if (ranger.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  /* Single Read */
  // this mode consume less power
  if ((millis()-lastTimeRanger) > loopDelayRanger) {
    distance = ranger.readRangeSingleMillimeters();
    Serial.print("Distance = "); Serial.println(distance);
    lastTimeRanger = millis();
    if (ranger.timeoutOccurred()) Serial.println("IR Ranger TIMEOUT");
  }
         
}
