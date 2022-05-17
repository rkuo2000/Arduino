#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <BleMouse.h>

BleMouse bleMouse;
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
int vx, vy;
int buttonL = 0;
int buttonR = 4;
int buttonLstate = 0; 
int buttonRstate = 0; 

void setup() {
  Serial.begin(115200); 
  Wire.begin();
  pinMode(buttonL, INPUT);
  pinMode(buttonR, INPUT);
  mpu.initialize();
  if (!mpu.testConnection()) { while (1); }
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  vx = (gx+15)/150;  
  vy = -(gz-100)/150;

  Serial.print("gx = ");
  Serial.print(gx);
  Serial.print(" | gz = ");
  Serial.print(gz);
  
  Serial.print("        | X = ");
  Serial.print(vx);
  Serial.print(" | Y = ");
  Serial.println(vy);
  
  bleMouse.move(vx, vy);
  
  buttonLstate = digitalRead(buttonL);
  buttonRstate = digitalRead(buttonR);
  
 if (buttonLstate == HIGH) {
    bleMouse.click(MOUSE_LEFT);
    delay(200);
  } 
  else if(buttonRstate == HIGH) {
    bleMouse.click(MOUSE_RIGHT);
    delay(200);
  }
  delay(80);
}
