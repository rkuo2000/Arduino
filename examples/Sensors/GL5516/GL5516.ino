/*
 * \brief Get light intensity in lux every second
 *
 * Schematics:
 *                           ^
 *            _____      ___/___
 *    5V |---|_____|----|__/____|--| GND
 *    or      10K        /
 *   3.3V                 GL5516
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 4 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 */
 
// default adc_resolution_bits = 10
#include "LightDependentResistor.h"
#define OTHER_RESISTOR 10000 // 10K ohms
#define USED_PIN A0 // NodeMCU = A0, RTL8720DN = A2
#define USED_PHOTOCELL LightDependentResistor::GL5516

// Create a GL5516 photocell instance (on A0 pin)
LightDependentResistor photocell(USED_PIN, OTHER_RESISTOR, USED_PHOTOCELL);

void setup(void)
{
  Serial.begin(115200);

  // Uncomment if your photocell is connected to 5V/3.3V instead of the other resistor
  //photocell.setPhotocellPositionOnGround(false);

  Serial.print("Light intensity will be given every second...\n");
}

void loop()
{
  // Check light intensity every second
  float intensity_in_lux = photocell.getCurrentLux();
  Serial.print("Light intensity: ");
  Serial.print(intensity_in_lux);
  Serial.print("lx (");
  Serial.print(LightDependentResistor::luxToFootCandles(intensity_in_lux));
  Serial.print("fc)\n");

  delay(1000);
}
