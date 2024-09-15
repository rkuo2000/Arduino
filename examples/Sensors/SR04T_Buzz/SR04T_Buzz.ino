// Sensor = JSN-SR04T + Buzzer : 防水超音波感測頭
// Blind spot <＝20cm

const int Trigger_pin = 18; // NodeMCU = D5, RTL8720DN = 12, NodeMCU-32S = 18
const int Echo_pin    = 19; // NodeMCU = D6, RTL8720DN = 11, NodeMCU-32S = 19
const int Buzz_pin    = 27; // NodeMCU-32S = 27

void setup() {
    Serial.begin(115200);
    pinMode(Trigger_pin, OUTPUT);
    pinMode(Echo_pin, INPUT);

    pinMode(Buzz_pin, OUTPUT);
    digitalWrite(Buzz_pin, HIGH);
}

void loop() {
    float duration, distance;

    digitalWrite(Trigger_pin, LOW);
    delayMicroseconds(2);

    // trigger a 10us HIGH pulse at trigger pin
    digitalWrite(Trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trigger_pin, LOW);

    // measure time cost of pulse HIGH at echo pin
    duration = pulseIn (Echo_pin, HIGH, 26000);

    // calculate the distance from duration
    distance = duration / 58;

    Serial.print(distance);
    Serial.println(" cm");

    // Buzzer
    if (distance==0) {
      digitalWrite(Buzz_pin, HIGH);      
    } else if (distance<65) {
      digitalWrite(Buzz_pin, LOW);  // low-active
      delay(20);
      digitalWrite(Buzz_pin, HIGH);
      delay(int(distance*5));
    } else if (distance<130) {
      digitalWrite(Buzz_pin, LOW);  // low-active
      delay(int(10));
      digitalWrite(Buzz_pin, HIGH);
      delay(int(distance*10));
    } else {
      digitalWrite(Buzz_pin, HIGH);
    }
}
