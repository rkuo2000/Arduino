// TB6612 full-bridge DC motor driver for ESP32
#include "ESP32_TB6612.h"
#include <Arduino.h>

Motor::Motor(int In1pin, int In2pin, int PWMpin, int PWMch, int offset, int STBYpin)
{
  In1 = In1pin;
  In2 = In2pin;
  PWM = PWMpin;
  channel = PWMch;
  Standby = STBYpin;
  Offset = offset;
  
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(Standby, OUTPUT);
  ledcSetup(channel, DEFAULTFREQ, 10);   
  ledcAttachPin(PWM, channel);  
}

void Motor::drive(int speed, int ch)
{
  digitalWrite(Standby, HIGH);
  speed = speed * Offset;
  if (speed>=0) fwd(speed, ch);
  else rev(-speed, ch);
}

//void Motor::drive(int speed, int duration)
//{
//  drive(speed);
//  delay(duration);
//}

void Motor::fwd(int speed, int ch)
{
   digitalWrite(In1, HIGH);
   digitalWrite(In2, LOW);
   ledcWrite(ch, speed);
}

void Motor::rev(int speed, int ch)
{
   digitalWrite(In1, LOW);
   digitalWrite(In2, HIGH);
   ledcWrite(ch, speed);   
}

void Motor::brake()
{
   digitalWrite(In1, LOW);
   digitalWrite(In2, LOW);
}

void Motor::standby()
{
   digitalWrite(Standby, LOW);
}

void forward(Motor motor1, Motor motor2, int speed)
{
	motor1.drive(speed, 0);
	motor2.drive(speed, 1);
}
void forward(Motor motor1, Motor motor2)
{
	motor1.drive(DEFAULTSPEED, 0);
	motor2.drive(DEFAULTSPEED, 1);
}


void backward(Motor motor1, Motor motor2, int speed)
{
	int temp = abs(speed);
	motor1.drive(-temp, 0);
	motor2.drive(-temp, 1);
}
void backward(Motor motor1, Motor motor2)
{
	motor1.drive(-DEFAULTSPEED, 0);
	motor2.drive(-DEFAULTSPEED, 1);
}
void left(Motor left, Motor right, int speed)
{
	int temp = abs(speed)/2;
	left.drive(-temp, 0);
	right.drive(temp, 1);
	
}


void right(Motor left, Motor right, int speed)
{
	int temp = abs(speed)/2;
	left.drive(temp, 0);
	right.drive(-temp, 1);
	
}
void brake(Motor motor1, Motor motor2)
{
	motor1.brake();
	motor2.brake();
}
