//
// HC-SR04 : Ultrasound Ranger (3cm~4m)
//
// Trig : Uno = D1, NodeMCU = D5 
// Echo : Uno = D2, NodeMCU = D6

#define TRIGGER D5
#define ECHO    D6

float duration, distance; 

void setup() { 
  Serial.begin (115200);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
}
 
void loop() {

  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2);   
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);   
  digitalWrite(TRIGGER, LOW);
  
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;  // (duration/2) * 34 /1000

  Serial.print(distance,2);
  Serial.println(" cm");
  delay(250);
}


