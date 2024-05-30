/*
   DRV8833 dual DC-Motors Driving Test
*/

// TB6612 pin connection
#define STBY  2
#define PWMA  3   // 1KHz
#define AIN2  4
#define AIN1  5
#define BIN1  6
#define BIN2  7
#define PWMB  8

void setup()
{
    Serial.begin(115200);
    // set 4 GPIOs to OUTPUT mode
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    analogWrite(PWMA, 250);
    analogWrite(PWMB, 250);
	
    Serial.println("TB6612 GPIO test with dual DC-motors");
    Serial.println("-------------------------------------");

    Serial.println("Motor0 & 1 Stop...");
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, LOW);
    delay(1000);

    Serial.println("Motor0 Rotating Clockwise...");
    digitalWrite(AIN1, HIGH); 
    digitalWrite(AIN2, LOW);
    delay(1000);
    Serial.println("Motor0 Rotating CounterClockwise...");
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, HIGH);
    delay(1000);

    Serial.println("Motor0 Stop...");
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, LOW);
    delay(1000);

    Serial.println("Motor1 Rotating Clockwise...");
    digitalWrite(BIN1, HIGH); 
    digitalWrite(BIN2, LOW);
    delay(1000);

    Serial.println("Motor1 Rotating CounterClockwise...");
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, HIGH);
    delay(1000);

    Serial.println("Motor1 Stop...");
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, LOW);
    delay(1000);
    
    // To switch clockwise and counterclosewise control, switch wiring or switch definition
}

void loop()
{
}
