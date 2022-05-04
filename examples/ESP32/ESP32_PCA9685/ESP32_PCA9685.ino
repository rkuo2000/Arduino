/*
  ESP32 PCA9685 Servo Control
  esp32-pca9685.ino
  Driving multiple servo motors with ESP32 and PCA9685 PWM module
  Use I2C Bus

  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/

// Include Wire Library for I2C
#include <Wire.h>

// Include Adafruit PCA9685 Servo Library
#include <Adafruit_PWMServoDriver.h>

// Creat object to represent PCA9685 at default I2C address
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

// Define maximum and minimum number of "ticks" for the servo motors
// Range from 0 to 4095
// This determines the pulse width

#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600  // Maximum value

// Define servo motor connections (expand as required)
#define SER0  0   //Servo Motor 0 on connector 0
#define SER1  12  //Servo Motor 1 on connector 12

// Variables for Servo Motor positions (expand as required)
int pwm0;
int pwm1;

void setup() {
  Serial.begin(115200);  // Serial monitor setup  
  Serial.println("PCA9685 Servo Test"); // Print to monitor  
  
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz
}

void loop() {
  // Move Motor 0 from 0 to 180 degrees
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {    
    pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(SER0, 0, pwm0); // Write to PCA9685
    Serial.print("Motor 0 = ");
    Serial.println(posDegrees);
    delay(30);
  }

  // Move Motor 1 from 180 to 0 degrees
  for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {    
    pwm1 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(SER1, 0, pwm1); // Write to PCA9685
    Serial.print("Motor 1 = "); 
    Serial.println(posDegrees);
    delay(30);
  }

  // Move Motor 0 from 180 to 0 degrees
  for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width
    pca9685.setPWM(SER0, 0, pwm0); // Write to PCA9685
    Serial.print("Motor 0 = ");
    Serial.println(posDegrees);
    delay(30);
  }

  // Move Motor 1 from 0 to 180 degrees
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    pwm1 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(SER1, 0, pwm1); // Write to PCA9685
    Serial.print("Motor 1 = ");
    Serial.println(posDegrees);
    delay(30);
  }
}
