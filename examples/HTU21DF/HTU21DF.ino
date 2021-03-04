// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: GIOP22)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: GIOP21)

#include <Wire.h>
#include <Adafruit_HTU21DF.h>

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void setup() {
  Serial.begin(115200);
  Serial.println("HTU21D-F test");

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
}

void loop() {
    float temp = htu.readTemperature();
    float humid = htu.readHumidity();
    Serial.print("Temperature: "); Serial.print(temp); Serial.print(" C");
    Serial.print("\t\t");
    Serial.print("Humidity: "); Serial.print(humid); Serial.println(" \%");
    delay(500);
}
