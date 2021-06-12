#include "ESP32_TB6612.h"

// TB6612 pin connection
#define STBY 23
#define PWMA  5   // 1KHz
#define AIN2 19
#define AIN1 18
#define BIN1 17
#define BIN2 16
#define PWMB  4

const int FULLSPEED = 1023;

// value 1 or -1 for motor spining default
const int offsetA = 1;
const int offsetB = 1;

Motor motorR = Motor(AIN1, AIN2, PWMA, 0, offsetA, STBY);
Motor motorL = Motor(BIN1, BIN2, PWMB, 1, offsetB, STBY);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 RoboCar TB6612 test"); 
}

void loop() {
  int Speed = FULLSPEED;
  
  forward(motorR, motorL, Speed);
  delay(500);
  brake(motorR, motorL);
  delay(500);
  backward(motorR, motorL, Speed);
  delay(500);
  brake(motorR, motorL);
  delay(500); 
}
