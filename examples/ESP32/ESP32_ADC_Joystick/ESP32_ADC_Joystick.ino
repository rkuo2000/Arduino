//
// Joystick 
// Vx = GIOP36(A0), Vy = GIOP39(A3), SW = GIOP12
//
const int pinSW = 12;

void setup(){
  Serial.begin(115200);
  pinMode(pinSW, INPUT_PULLUP);
}

void loop() {
  Serial.print(digitalRead(pinSW));
  Serial.print("\t");  
  Serial.print(analogRead(A0));
  Serial.print("\t");
  Serial.println(analogRead(A3)); 
  delay(100);
}
