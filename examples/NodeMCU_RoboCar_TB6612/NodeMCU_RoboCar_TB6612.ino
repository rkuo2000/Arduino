#include <ESP8266WiFi.h>
#include "SparkFun_TB6612.h"

// TB6612FNG pin connection
#define STBY D0
#define PWMA D3
#define AIN2 D4
#define AIN1 D5
#define BIN1 D6
#define BIN2 D7
#define PWMB D8

// PWM Full Speed value
// PWM freq : NodeMCU = 1KHz, UNO = 500Hz
// PWM duty   NodeMCU = 1023 (10-bit PWM), UNO = 255 (8-bit PWM)

const int FULLSPEED = 1023;

// value 1 or -1 for motor spining default
const int offsetA = 1;
const int offsetB = 1;

Motor motorR = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motorL = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

void setup() {
  Serial.begin(115200);
  Serial.println("NodeMCU RoboCar TB6612 test");
  brake(motorR, motorL);
}

void loop() {
  forward(motorR, motorL, FULLSPEED);
  delay(500);
  brake(motorR, motorL);
  backward(motorR, motorL, FULLSPEED);
  delay(500);
  brake(motorR, motorL);
  right(motorR, motorL, FULLSPEED);
  delay(500);
  brake(motorR, motorL);
  left(motorR, motorL, FULLSPEED);
  delay(500);
  brake(motorR, motorL);  
  while(1){};
}
