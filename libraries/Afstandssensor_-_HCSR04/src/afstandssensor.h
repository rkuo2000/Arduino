/*
  HCSR04 - Arduino bibliotek for HCSR04 ultrasonic afstandssensor
  Lavet af Jørgen Larsen, 30 Oktober 2019.
  Inspireret af Martin Sosic, June 11, 2016.
*/

#ifndef AFSTANDSSENSOR_H
#define AFSTANDSSENSOR_H

#include "Arduino.h"

class AfstandsSensor {
 public:
    /**
     * @param triggerPin  Digital ben til styring af sensoren (output).
     * @param echoPin  Digitalt ben til modtagelse af data fra sensoren (input).
     */
    AfstandsSensor(int triggerPin, int echoPin);

    /**
     * Måler afstanden
     * @returns Afstanden i cm. Ved afstande længere end 400cm retuneres negativ værdi.
     */
    double afstandCM();

    /**
     * Måler afstanden
     * @returns Afstanden i cm. Ved afstande længere end 400cm retuneres negativ værdi.
     * @param temperatur  Temperatur i grades celcius
     */
    double afstandCM(float temperatur);
 private:
    int triggerPin, echoPin;
};

#endif // AFSTANDSSENSOR_H
