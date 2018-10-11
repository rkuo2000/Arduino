#include <Arduino.h>

#define LED_R D6
#define LED_G D8
#define LED_B D7

void setup()
{
  Serial.begin(115200);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);    
}

void color_led(int r, int g, int b)
{
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);    
}

void loop()
{
  int r, g, b;
  for (r=256; r>=0; r=r-16) {
    if (r==256) r=255;
    analogWrite(LED_R, r);
    analogWrite(LED_B, 255-r);
    analogWrite(LED_G, 0);
    delay(500);
  }
  
  for (b=256; b>=0; b=b-16) {
    if (b==256) b=255;
    analogWrite(LED_B, b);
    analogWrite(LED_G, 255-b);
    analogWrite(LED_R, 0);    
    delay(500);
  }

  for (g=256; g>=0; g=g-16) {
    if (g==256) g=255;
    analogWrite(LED_G, g);
    analogWrite(LED_R, 255-g);
    analogWrite(LED_B, 0);           
    delay(500);
  }
  
/* Random  
  r = random(0, 255);
  g = random(0, 255);
  b = random(0, 255);
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
  delay(1000);
*/  
}
