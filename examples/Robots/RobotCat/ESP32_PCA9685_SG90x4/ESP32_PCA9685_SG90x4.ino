// Sweep All-four Legs' Upper/Middle/Lower Servo Motor (on connector 3,7,11,15)

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600 // Maximum value

// Define servo motor connections (expand as required)
#define SERVO_SG90_0 3  // one SG90 Servo Motor on connector 3
#define SERVO_SG90_1 7  // one SG90 Servo Motor on connector 7
#define SERVO_SG90_2 11 // one SG90 Servo Motor on connector 11
#define SERVO_SG90_3 15 // one SG90 Servo Motor on connector 15

// Variables for Servo Motor positions (expand as required)
int posPulse;

void setup() {
  Serial.begin(115200);  // Serial monitor setup  
  Serial.println("PCA9685 Servo Test"); // Print to monitor  
  
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz
}

void loop() {
  // Move Motor 0 from 0 to 180 degrees
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {    
    int posPulse = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(SERVO_SG90_0, 0, posPulse); // Write to PCA9685
    pca9685.setPWM(SERVO_SG90_1, 0, posPulse); // Write to PCA9685
    pca9685.setPWM(SERVO_SG90_2, 0, posPulse); // Write to PCA9685
    pca9685.setPWM(SERVO_SG90_3, 0, posPulse); // Write to PCA9685            
    Serial.print("Servo SG90 = ");
    Serial.println(posDegrees);
    delay(30);
  }
  delay(100);
}
