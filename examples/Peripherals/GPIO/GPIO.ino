#define pin 23

void setup(){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void loop() {
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
  delay(1000);
}
