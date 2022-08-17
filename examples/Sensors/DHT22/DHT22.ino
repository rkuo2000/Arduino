// DHT22 pinout : Vcc, DATA, N.C., GND
#include "DHT.h"

#define DHTPIN 8     // NodeMCU = D6, RTL8720DN = 8
DHT dht(DHTPIN, DHT22, 15);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  delay(1000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Temp: "); Serial.print(t); Serial.print("°C");
  Serial.print("\t");
  Serial.print("Humidity: "); Serial.print(h); Serial.println("\%");  
  
  // for ESP32 go to deepsleep for 1 minutes
  //system_deep_sleep_set_option(0);
  //system_deep_sleep(1 * 60 * 1000000);
}
