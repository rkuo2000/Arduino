#include <ESP32Servo.h>

const int servoPin = 23;

Servo bldc;

int pos = 0; // degree

void setup() {  
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  bldc.setPeriodHertz(50);           // set freq to 50Hz
  bldc.attach(servoPin, 1000, 2000); // pin, min, max
  bldc.write(90);
  delay(1000);
  bldc.write(0);
}

void loop() {  
  bldc.write(pos);
  delay(5000);
  pos = pos + 45;
  if (pos>180) pos=0;
}
