#include "I2Cdev.h"
#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>

BleMouse bleMouse;
MPU6050 imu;
int16_t ax, ay, az, gx, gy, gz;
int vx, vy;
int buttonL = 0; // IO0 button
int buttonR = 4;
int buttonLstate = HIGH; 
int buttonRstate = HIGH; 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(buttonL, INPUT_PULLUP);
  pinMode(buttonR, INPUT_PULLUP);
  
  Serial.print("MPU6050 initializing");
  imu.initialize();
  while (!imu.testConnection()) { Serial.print("."); }
  Serial.println();  
  Serial.println("BLE Mouse starting !");
  bleMouse.begin();
}

void loop() {
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  vx =  (gx+15)/150;  
  vy = -(gy-100)/150;
  
  Serial.print("gx = ");   Serial.print(gx);
  Serial.print(", gy = "); Serial.print(gy);
  Serial.print("\t");
  Serial.print("X = ");    Serial.print(vx);
  Serial.print(", Y = ");  Serial.println(vy);
  
  bleMouse.move(vx, vy);
    
  buttonLstate = digitalRead(buttonL);
  buttonRstate = digitalRead(buttonR);  
  
  if (buttonLstate == LOW) { // press button to Ground
    bleMouse.click(MOUSE_LEFT);
    delay(100);
  } 
  else if(buttonRstate == LOW) { // press button to Ground
    bleMouse.click(MOUSE_RIGHT);
    delay(100);
  }
  delay(100);
}
