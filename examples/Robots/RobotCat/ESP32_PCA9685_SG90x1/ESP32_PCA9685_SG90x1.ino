// Sweep one SG90 Servo motor on PCA9685 pwm-connector #15

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  80  // Minimum value
#define SERVOMAX  600 // Maximum value

void setup() {
  Serial.begin(115200);  // Serial monitor setup  
  Serial.println("PCA9685 Servo Test"); // Print to monitor  
  
  pca9685.begin(); // Initialize PCA9685  
  pca9685.setPWMFreq(50); // Set PWM Frequency to 50Hz
}

void sweep(int pinSERVO) {
  // Move Motor 0 from 0 to 180 degrees
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {    
    int posPulse = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX); // Determine PWM pulse width    
    pca9685.setPWM(pinSERVO, 0, posPulse); // Write to PCA9685           
    Serial.print("Servo "); Serial.print(pinSERVO); 
    Serial.print(" = ");    Serial.print(posDegrees);
    Serial.print(", pulse ="); Serial.println(posPulse);
    delay(30);   
  }  
}

void loop() {
  sweep(15); //sweep Servo Motor on connector 15
  delay(300);
}
