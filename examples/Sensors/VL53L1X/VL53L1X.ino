// VL531LX is an infrared ranger
// max distance // < 4 meter
// this example use continous mode to take range measurements with the VL53L1X

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: GIOP22, RTL8720DN: PA25)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: GIOP21, RTL8720DN: PA26))

#include <Wire.h>
#include <VL53L1X.h> // pololu v1.3.0

VL53L1X sensor;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())  
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(50);
}

void loop()
{
  Serial.print(sensor.read());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
