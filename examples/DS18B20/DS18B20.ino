#include <OneWire.h>
#include <DallasTemperature.h>

// Connection
#define ONE_WIRE_BUS D4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Temperature Sensor");
  sensors.begin();
}

void loop(void)
{
  sensors.requestTemperatures(); // Send the command to get temperature reading
  Serial.println(sensors.getTempCByIndex(0));
  delay(1000);
}
