
#include "DHT.h"

#define DHTPIN D6     // NodeMCU pin D6 connected to DHT22 pin Data
DHT dht(DHTPIN, DHT22, 15); //DHT22

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  dht.begin();
  delay(10);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Temp: "); Serial.print(t); Serial.print("°C");
  Serial.print("\t");
  Serial.print("Humidity: "); Serial.print(h); Serial.println("\%");  
  
  // go to deepsleep for 1 minutes
  //system_deep_sleep_set_option(0);
  //system_deep_sleep(1 * 60 * 1000000);
  delay(1*60*1000);
}
