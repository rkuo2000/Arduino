// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// I2C to VL53L0X InfraRed ranger

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: GIOP22)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: GIOP21)

#include <Wire.h>
#include "VL53L0X.h"
#include "ESP32MotorControl.h"

VL53L0X ranger;

// DRV8833 pin connection
#define IN1pin 19  
#define IN2pin 18  
#define IN3pin 23 
#define IN4pin 19

#define motorR 0
#define motorL 1

ESP32MotorControl motor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  ranger.init();
  ranger.setTimeout(500);
  ranger.startContinuous();  
    
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);

  motor.motorStop(motorR);
  motor.motorStop(motorL);
}

void loop() {
  int distance = ranger.readRangeContinuousMillimeters() - 50;
  Serial.print(distance);
  Serial.println(" mm");
  if (ranger.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  if (distance<30) {
    motor.motorStop(0);
    motor.motorStop(1);     
  } else {
    motor.motorForward(motorR, 100);
    motor.motorForward(motorL, 100);
  }
  
  delay(100);
}
