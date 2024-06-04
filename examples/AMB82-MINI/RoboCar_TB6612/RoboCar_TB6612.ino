
// L7805CV pin connection
// pin 1 : Battery V+
// pin 2 : Battery V-, TB6612's Gnd
// pin 3 : +5V

// TB6612 pin connection

// VM  connected to +5V 
// Vcc connected to +3.3V or +5V
// Gnd connected to Ground
#define PWMA  8
#define AIN2  7
#define AIN1  6
#define STBY  5
#define BIN1  4
#define BIN2  3
#define PWMB  2

#define FULLSPEED 255
#define HALFSPEED 128
#define STOPSPEED 0

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  

  Serial.println("TB6612FNG Pin setup...");

  pinMode(PWMA, OUTPUT);  
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  // set default value
  digitalWrite(AIN2, LOW);
  digitalWrite(AIN1, LOW);
  digitalWrite(STBY, HIGH); // LOW = standby
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("TB6612FNG Test begin !!!");

  Serial.println("Set PWM value for Motor Speed !");
  //set Motor Speed
  analogWrite(PWMA, 250);
  analogWrite(PWMB, 250);

  Serial.println("Forward...");

  forward();
  delay(1000);
  stop();
  delay(1000);

  Serial.println("Backward...");
  backward();
  delay(1000);
  stop();
  delay(1000);

  Serial.println("Turn-Left...");   
  turnleft();
  delay(1000);
  stop();
  delay(1000);

  Serial.println("Turn-Right...");  
  turnright();
  delay(1000);
  stop();  
  delay(1000);
  Serial.println("Test finished !!!");  
  digitalWrite(LED_BUILTIN, LOW);  
}

void forward(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void backward(){
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void turnleft(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void turnright(){
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void stop(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void loop() {
}
