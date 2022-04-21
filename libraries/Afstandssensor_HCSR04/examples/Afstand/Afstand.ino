#include <afstandssensor.h>

                                        // AfstandsSensor(triggerPin, echoPin);
AfstandsSensor afstandssensor(13, 12);  // Starter afstandssensoren på ben 13 og 12.

void setup () {
    Serial.begin(9600);  // Opsætter serial kommunikation tilbage til computeren
}

void loop () {
    // Måler afstanden for hver 500ms
    Serial.println(afstandssensor.afstandCM());
    delay(500);
}
