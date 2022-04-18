// MG996R DC Servo (metal gear)

// Speed:  4.8V @0.2sec / 60 degree
//         6.0V @0.19sec / 60 degree
// Torque: 4.8V @13kg-cm
//         6.0V @15kg-cm
// VCC : 4.8~7.2V
#include <Servo.h>

Servo myservo;  // create servo object (max. 12 servo)

int pos = 0;    // Servo position

const int PWMpin = D1;   // NodeMCU PWM1~8 = D1~D8

void setup() {
  myservo.attach(PWMpin);  // servo sig pin connected to PWMpin
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) // rotate from 0 to 180 degree
  {     
    myservo.write(pos);               // output to set servo position
    delay(15);                        // wait for 15ms
  }

  for (pos = 180; pos >= 0; pos -= 1) // rotate from 180 to 0 degree
  { 
    myservo.write(pos);               // output to set servo position
    delay(15);                        // wait for 15ms 
  }
}
