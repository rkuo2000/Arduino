// MLX90614_OLED : ForeHead Thermometer
// MLX90614 : InfraRed Temperature Sensor
// SSD1306  : 0.9" OLED display

#include <Wire.h>
#include "Adafruit_MLX90614.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define AVERAGE_No 50
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET); // 0.9" OLED 128x64: Width = 128, Height = 64

float temp;
float object;

void setup() {
  int i;
  Serial.begin(115200);
  Wire.begin(D2, D1); // I2C

  // OLED display Init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C
  display.clearDisplay(); // Clear the buffer.
  display.setTextSize(2); // set Text Size

  // OLED display print
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("ForeHead");
  display.println("Scanner");
  display.display();
  delay(2000); // Pause for 2 seconds

  // MLX90614 Init
  mlx.begin();  
  // read & accumulate the object temperatures
  for (i=0; i<AVERAGE_No; i++) {
    temp = temp + mlx.readObjectTempC();
  }
  // calculate average object temperature
  temp=temp/AVERAGE_No; 
  Serial.print("T = "); Serial.print(temp); Serial.print("ºC");
}

void loop() {
  // MLX90614 sensor
  object=mlx.readObjectTempC();  
  Serial.print("T = "); Serial.print(object); Serial.print("ºC");
  Serial.println();  

  // OLED display
  display.clearDisplay(); // Clear the buffer.
  display.setCursor(0,0);
  display.println("ForeHead");
  display.println("Scanner");
  display.print("T = "); display.print(object); 
  display.display(); 
}
