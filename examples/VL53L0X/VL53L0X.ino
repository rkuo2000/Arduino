// VL530LX is an infrared ranger
// max distance // < 2 meter
// this example use continous mode to take range measurements with the VL53L0X
#include <Wire.h>
#include "VL53L0X.h"

VL53L0X sensor;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  sensor.init();
  sensor.setTimeout(500);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous();
}

void loop()
{
  Serial.print(sensor.readRangeContinuousMillimeters());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println(" mm");
}
