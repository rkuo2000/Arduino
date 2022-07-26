// Servo motor on connector (0,1,2) (4,5,6) (8,9,10), (12,13,14) (NC : 3,7,11,15)
// Initialize pose=0 for each leg (one at a time)

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600 // Maximum value

// Define servo motor connections (expand as required)
#define SERVO_LFu  0 // Left-Front Leg Servo Motor 0 on connector 0
#define SERVO_LFm  1 // Left-Front Leg Servo Motor 1 on connector 1
#define SERVO_LFl  2 // Left-Front Leg Servo Motor 2 on connector 2

#define SERVO_RFu  4 // Right-Front Leg Servo Motor 0 on connector 4
#define SERVO_RFm  5 // Right-Front Leg Servo Motor 1 on connector 5
#define SERVO_RFl  6 // Right-Front Leg Servo Motor 2 on connector 6

#define SERVO_LBu  8 // Left-Front Leg Servo Motor 0 on connector 8
#define SERVO_LBm  9 // Left-Front Leg Servo Motor 1 on connector 9
#define SERVO_LBl  10 // Left-Front Leg Servo Motor 2 on connector 10

#define SERVO_RBu  12 // Right-Front Leg Servo Motor 0 on connector 12
#define SERVO_RBm  13 // Right-Front Leg Servo Motor 1 on connector 13
#define SERVO_RBl  14 // Right-Front Leg Servo Motor 2 on connector 14

// Variables for Servo Motor positions (expand as required)
int posPulse;

void setposSERVO(int pinSERVO, int pos) {
  posPulse = map(pos, 0, 180, SERVOMIN, SERVOMAX);
  pca9685.setPWM(pinSERVO, 0, posPulse); // Write to PCA9685
  delay(100);
}

void setup() {
  Serial.begin(115200);  // Serial monitor setup  
  Serial.println("PCA9685 Servo Test"); // Print to monitor  
  
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz

  setposSERVO(SERVO_LFu, 0);
  pca9685.setPin(SERVO_LFu, 0);
  setposSERVO(SERVO_LFm, 0);
  pca9685.setPin(SERVO_LFm, 0);  
  setposSERVO(SERVO_LFl, 180);
  pca9685.setPin(SERVO_LFl, 0);
  
  setposSERVO(SERVO_RFu, 180);
  pca9685.setPin(SERVO_RFu, 0);  
  setposSERVO(SERVO_RFm, 180);
  pca9685.setPin(SERVO_RFm, 0);   
  setposSERVO(SERVO_RFl, 0);  
  pca9685.setPin(SERVO_RFl, 0);

  setposSERVO(SERVO_LBu, 0);
  pca9685.setPin(SERVO_LBu, 0);
  setposSERVO(SERVO_LBm, 0);
  pca9685.setPin(SERVO_LBm, 0);  
  setposSERVO(SERVO_LBl, 180);
  pca9685.setPin(SERVO_LBl, 0);
  
  setposSERVO(SERVO_RBu, 180);
  pca9685.setPin(SERVO_RBu, 0);  
  setposSERVO(SERVO_RBm, 180);
  pca9685.setPin(SERVO_RBm, 0);   
  setposSERVO(SERVO_RBl, 0);  
  pca9685.setPin(SERVO_RBl, 0);      
}

void loop() {  
}
