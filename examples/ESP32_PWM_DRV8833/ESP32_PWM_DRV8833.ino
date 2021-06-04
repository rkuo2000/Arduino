// PWM to DRV8833 dual H-bridge motor driver
// PWM freq. = 1000 Hz
#include "ESP32MotorControl.h"

ESP32MotorControl motor;

// DRV8833 pin connection
#define IN1pin 0  
#define IN2pin 0  
#define IN3pin 23 
#define IN4pin 19 

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 RoboCar DRV8833 test");
  
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);
}

void loop() {
  Serial.println("test motorForward");
  motor.motorForward(1, 100);  
  delay(500);    
  motor.motorStop(1); 
  delay(1000);    
  Serial.println("test motorReverse");  
  motor.motorReverse(1, 100);  
  delay(500);
  Serial.println("test motorStop");   
  motor.motorStop(1); 
  delay(1000);   
}
