/*

*/

// DRV8833 dual DC-motor driver IC
// nSLEEP pin (EEP) connect to Vcc (+5V), if connected to GND, then chip will enter low-power mode
// nFAULT pin keep floating
#define IN1_PIN     5 // in1 pin from one of the two DRV8833 H-bridge
#define IN2_PIN     6 // in2 pin from one of the two DRV8833 H-bridge
#define IN3_PIN     7 // in3 pin from one of the two DRV8833 H-bridge
#define IN4_PIN     8 // in4 pin from one of the two DRV8833 H-bridge

void setup()
{
    Serial.begin(115200);

    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);

    Serial.println("L298 tester");
    Serial.println("--------------");
    Serial.print("Motor0: ");
    Serial.print("IN1 pin= "); Serial.print(IN1_PIN); Serial.print(",\t");
    Serial.print("IN2 pin= "); Serial.print(IN2_PIN); Serial.print(";\t");
    Serial.print("Motor1: ");
    Serial.print("IN3 pin= "); Serial.print(IN3_PIN); Serial.print(",\t");
    Serial.print("IN4 pin= "); Serial.print(IN4_PIN); Serial.println(";\t");

    Serial.println();

    Serial.println("Motor0 & 1 Stop...");
    digitalWrite(IN1_PIN, LOW); 
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW); 
    digitalWrite(IN4_PIN, LOW);
    delay(1000);

    Serial.println("Motor0 Clockwise...");
    digitalWrite(IN1_PIN, HIGH); 
    digitalWrite(IN2_PIN, LOW);
    delay(1000);

    Serial.println("Motor0 CounterClockwise...");
    digitalWrite(IN1_PIN, LOW); 
    digitalWrite(IN2_PIN, HIGH);
    delay(1000);

    Serial.println("Motor0 Stop...");
    digitalWrite(IN1_PIN, LOW); 
    digitalWrite(IN2_PIN, LOW);
    delay(1000);

    Serial.println("Motor1 Clockwise...");
    digitalWrite(IN3_PIN, HIGH); 
    digitalWrite(IN4_PIN, LOW);
    delay(1000);

    Serial.println("Motor1 CounterClockwise...");
    digitalWrite(IN3_PIN, LOW); 
    digitalWrite(IN4_PIN, HIGH);
    delay(1000);

    Serial.println("Motor1 Stop...");
    digitalWrite(IN3_PIN, LOW); 
    digitalWrite(IN4_PIN, LOW);
    delay(1000);
}

void loop()
{
}