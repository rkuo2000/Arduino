/*
  HCSR04 - Arduino bibliotek for HCSR04 ultrasonic afstandssensor
  Lavet af Jørgen Larsen, 30 Oktober 2019.
  Inspireret af Martin Sosic, June 11, 2016.
*/

#include "Arduino.h"
#include "afstandssensor.h"

AfstandsSensor::AfstandsSensor (
        int triggerPin, int echoPin) {
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

double AfstandsSensor::afstandCM() {
    //Afstanden måles ud fra en standrad temperatur på 19.307 grader celcius, hvilket svare ca til 343m/s for lyd.
    return afstandCM(19.307);
}

double AfstandsSensor::afstandCM(float temperature) {
    // Sikre først at triggerPin er LAV
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    //For at aktivere sensoren holdes triggerPin HØJ i 10 microsekunder.
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    //Sensoren retunere nu afstanden med at holde echoPin HØJ i en periode svarende til afstanden målt.
    unsigned long durationMicroSec = pulseIn(echoPin, HIGH);

    double speedOfSoundInCmPerMs = 0.03313 + 0.0000606 * temperature; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
    double distanceCm = durationMicroSec / 2.0 * speedOfSoundInCmPerMs;
    if (distanceCm == 0 || distanceCm > 400) {
        return -1.0 ;
    } else {
        return distanceCm;
    }
}
