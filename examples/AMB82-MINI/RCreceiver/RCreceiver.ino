#define RCPin 2
int RCValue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RCPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  RCValue = pulseIn(RCPin, HIGH);
  Serial.println(RCValue);
}
