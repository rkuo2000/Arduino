//
// Ultrasound Ranger (3cm~4m)
//
#include <afstandssensor.h>

                                        // AfstandsSensor(triggerPin, echoPin);
AfstandsSensor afstandssensor(13, 12);  // pin: Trig = 13, Echo = 12
void setup () {
    Serial.begin(115200);
}

void loop () {
    Serial.print(afstandssensor.afstandCM()); 
    Serial.println(" cm");
    delay(500);
}
