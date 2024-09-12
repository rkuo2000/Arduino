/*
 * Demonstrates use of HC-SR04 ultrasonic range module
 *
 * This sketch sends a 10us pulse HIGH at trigger pin of HC-SR04.
 * HC-SR04 return a pulse HIGH at echo pin which corresponds the distance.
 *     Time = Width of Echo pulse, in us (micro second)
 *     Distance in centimeters = Time / 58
 *
 * HC-SR04 works at 5V domain.
 * It means the echo pin needs level shift from 5V to 3.3V.
 * We can either use a level converter or use resister to devide the level.
 *
 **/

const int trigger_pin = 18; // NodeMCU = D5, RTL8720DN = 12, NodeMCU-32S = 18
const int echo_pin    = 19; // NodeMCU = D6, RTL8720DN = 11, NodeMCU-32S = 19
const int Buzz_pin    = 27; // NodeMCU-32S = 27

void setup() {
    Serial.begin(115200);
    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);

    pinMode(Buzz_pin, OUTPUT);
    digitalWrite(Buzz_pin, HIGH);
}

void loop() {
    float duration, distance;

    // trigger a 10us HIGH pulse at trigger pin
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    // measure time cost of pulse HIGH at echo pin
    duration = pulseIn (echo_pin, HIGH);

    // calculate the distance from duration
    distance = duration / 58;

    Serial.print(distance);
    Serial.println(" cm");

    // Buzzer
    if (distance<100) {
      digitalWrite(Buzz_pin, LOW);  // low-active
      delay(20);
      digitalWrite(Buzz_pin, HIGH);
      delay(int(distance*5));
    } else if (distance<200) {
      digitalWrite(Buzz_pin, LOW);  // low-active
      delay(int(10));
      digitalWrite(Buzz_pin, HIGH);
      delay(int(distance*10));
    }
}
