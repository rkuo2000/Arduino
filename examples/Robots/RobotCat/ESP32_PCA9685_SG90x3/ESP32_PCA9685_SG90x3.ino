// Sweep One Leg Upper/Middle/Lower Servo Motor

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600 // Maximum value

// Define servo motor connections (expand as required)
#define SERVO_LFU 0  // one SG90 Servo Motor on connector 0
#define SERVO_LFM 1  // one SG90 Servo Motor on connector 1
#define SERVO_LFL 2  // one SG90 Servo Motor on connector 2

#define SERVO_RFU 4  // one SG90 Servo Motor on connector 4
#define SERVO_RFM 5  // one SG90 Servo Motor on connector 5
#define SERVO_RFL 6  // one SG90 Servo Motor on connector 6

#define SERVO_LBU 8  // one SG90 Servo Motor on connector 8
#define SERVO_LBM 9  // one SG90 Servo Motor on connector 9
#define SERVO_LBL 10  // one SG90 Servo Motor on connector 10

#define SERVO_RBU 12  // one SG90 Servo Motor on connector 12
#define SERVO_RBM 13 // one SG90 Servo Motor on connector 13
#define SERVO_RBL 14 // one SG90 Servo Motor on connector 14

// Variables for Servo Motor positions (expand as required)
int posPulse;

void setup() {
  Serial.begin(115200);  // Serial monitor setup  
  Serial.println("PCA9685 Servo Test"); // Print to monitor  
  
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz
}

void sweep(int pinSERVO) {
  // Move Motor 0 from 0 to 180 degrees
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {    
    posPulse = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(pinSERVO, 0, posPulse); // Write to PCA9685           
    Serial.print("Servo "); Serial.print(pinSERVO); 
    Serial.print(" = ");    Serial.println(posDegrees);
    delay(30);
  }  
}
  
void loop() {
  // Sweep Left-Front Motors
  sweep(SERVO_LFU);
  pca9685.setPin(SERVO_LFU, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_LFM);
  pca9685.setPin(SERVO_LFM, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_LFL);
  pca9685.setPin(SERVO_LFL, 0); // set PWM pin output to 0    
  delay(100);
/*
  // Sweep Right-Front Motors
  sweep(SERVO_RFU);
  pca9685.setPin(SERVO_RFU, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_RFM);
  pca9685.setPin(SERVO_RFM, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_RFL);
  pca9685.setPin(SERVO_RFL, 0); // set PWM pin output to 0    
  delay(100);

  // Sweep Left-Back Motors
  sweep(SERVO_LBU);
  pca9685.setPin(SERVO_LBU, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_LBM);
  pca9685.setPin(SERVO_LBM, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_LBL);
  pca9685.setPin(SERVO_LBL, 0); // set PWM pin output to 0    
  delay(100);

  // Sweep Right-Back Motors
  sweep(SERVO_RBU);
  pca9685.setPin(SERVO_RBU, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_RBM);
  pca9685.setPin(SERVO_RBM, 0); // set PWM pin output to 0
  delay(100);
  sweep(SERVO_RBL);
  pca9685.setPin(SERVO_RBL, 0); // set PWM pin output to 0    
  delay(100);
*/  
}
