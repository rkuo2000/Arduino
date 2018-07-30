/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
/* 
 I2C connection
 D1 = SCL 
 D2 = SDA 
*/
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}
#include <Wire.h>
#include <Adafruit_MLX90614.h>

WiFiClient client;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* ssid     = "Kuo"; 
const char* password = "0972211921";

const char* secret = "ssJGT3gEh6QJCH2Kfmhdpnq112A6t";
const char* src_addr = "69AmkbuBTyDsSogtdXyBQBb1QMydMxnceH";
const char* dest_addr = "6BPBx57t3YCTyCAZc4UEMKdqEQwF12PQSQ";

const char* transfer_amount = "0.000001"; // SDA
// transaction fee = 0.0001 SDA

const char* url_getBalance = "https://rest-beta.sdchain.io/v1/accounts/balances/";
const char* url_postPayment = "https://rest-beta.sdchain.io/v1/accounts/payments/";

void setup() {
  Serial.begin(9600);
  
/* connecting WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Adafruit MLX90614 test");  
/* begin MLX90614 */
  mlx.begin();  
}

void loop() {
  
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  
/* access SDChain */
  Serial.print("getBalance of DestAddr\n\r");
  client.print(String("GET ") + url_getBalance +dest_addr );
  while (client.connected())
  {
    Serial.print("reading client.....\n\r");
    if (client.available())
    {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }   
 
}
