
#include "DHT.h"

#define DHTPIN 8     // NodeMCU = D6, RTL8720DN = 8
DHT dht(DHTPIN, DHT11, 15); 

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
}
