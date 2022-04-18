// TB6612 full-bridge DC motor for ESP32
#include "ESP32_TB6612.h"

// TB6612 pin connection
#define STBY 23
#define PWMA  5   // 1KHz
#define AIN2 19
#define AIN1 18
#define BIN1 17
#define BIN2 16
#define PWMB  4

#define FULLSPEED 1023
#define HALFSPEED 512

// value 1 or -1 for motor spining default
const int offsetA = 1;
const int offsetB = 1;

Motor motorR = Motor(AIN1, AIN2, PWMA, 0, offsetA, STBY);
Motor motorL = Motor(BIN1, BIN2, PWMB, 1, offsetB, STBY);

int Speed = HALFSPEED;

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Forward");
  forward(motorR, motorL, Speed); 
  delay(1000); // 1 sec
  
  Serial.println("Backward");
  backward(motorR, motorL, Speed);
  delay(1000); // 1 sec
  
  Serial.println("Right");
  right(motorR, motorL, Speed); 
  delay(1000); // 1 sec
  
  Serial.println("Left");
  left(motorR, motorL, Speed);
  delay(1000); // 1 sec
      
  Serial.println("Stop");  
  brake(motorR, motorL);
  delay(2000); // 2 sec  
}
