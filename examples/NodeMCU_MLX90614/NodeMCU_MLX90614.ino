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

// NodeMCU use 3 pairs of I2C pins for three MLX90614 temp sensors
#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define AVERAGE_No 50
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float temp[3] = {0, 0, 0};
float object[3];

void setup() {
  int i;
  Serial.begin(115200);
  Serial.println("Adafruit MLX90614 test");
  mlx.begin();
  
  // read & accumulate multiple object temperatures
  for (i=0; i<AVERAGE_No; i++) {
    Wire.begin(D2, D1);
    temp[0] = temp[0] + mlx.readObjectTempC();
    Wire.begin(D4, D3)
    temp[1] = temp[1] + mlx.readObjectTempC();
    Wire.begin(D4, D3)
    temp[2] = temp[2] + mlx.readObjectTempC();
  }
  // calculate average object temperature
  temp[0]=temp[0]/AVERAGE_No; 
  temp[1]=temp[1]/AVERAGE_No;
  temp[2]=temp[2]/AVERAGE_No;
}

void loop() {
// first MLX90614 sensor
  Wire.begin(D2, D1);
  object[0]=mlx.readObjectTempC();  
  Serial.print("1st T="); Serial.print(object[0]); Serial.print("*C ");

// second MLX90614 sensor
  Wire.begin(D4, D3);
  object[1]=mlx.readObjectTempC();
  Serial.print("2nd T="); Serial.print(object[1]); Serial.print("*C "); 
  
// third MLX90614 sensor
  Wire.begin(D6, D5);
  object[2]=mlx.readObjectTempC();  
  Serial.print("3rd T="); Serial.print(object[2]); Serial.print("*C ");

  Serial.println();
  
  Serial.print("Detect:");
  if ((object[0]-temp[0])>1) Serial.print(" Sensor1");
  else                       Serial.print(" Nothing");
  if ((object[1]-temp[1])>1) Serial.print(" Sensor2");
  else                       Serial.print(" Nothing");  
  if ((object[2]-temp[2])>1) Serial.print(" Sensor3");
  else                       Serial.print(" Nothing");
  Serial.println();
  
  delay(500);  
}
