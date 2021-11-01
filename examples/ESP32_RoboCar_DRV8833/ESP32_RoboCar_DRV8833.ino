// EPS32 PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz

#include <ESP32MotorControl.h> 

// DRV8833 pin connection
#define IN1pin 16  
#define IN2pin 17  
#define IN3pin 18 
#define IN4pin 19

#define motorR 0
#define motorL 1

#define FULLSPEED 100
#define HALFSPEED 50

ESP32MotorControl motor;

int speed = HALFSPEED;

void setup() {
  Serial.begin(115200);
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);

  motor.motorStop(motorR);
  motor.motorStop(motorL);
}

void loop() {
  Serial.println("Move Forward");
  motor.motorForward(motorR, speed);  
  motor.motorForward(motorL, speed);
  delay(1000); // 1 sec

  Serial.println("Move Stop");  
  motor.motorStop(motorR);  
  motor.motorStop(motorL);
  delay(1000); // 1 sec
  
}

