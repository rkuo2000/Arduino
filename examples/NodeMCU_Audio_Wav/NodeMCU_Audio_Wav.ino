//
// NodeMCU play wave from PROGMEM
//
// NodeMCU connections:
// Vin (5V)      to Speaker+
//      Speaker- to 2N3904-Collector          
// RX (I2Sout)   to 2N3904-Base
// Gnd           to 2N3904-Emitter

#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

#include "cat_meow.h"

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2SNoDAC *out;

void setup()
{
  WiFi.mode(WIFI_OFF); 
  Serial.begin(115200);
  delay(1000);
  Serial.printf("WAV start\n");
  file = new AudioFileSourcePROGMEM( cat_meow_wav, sizeof(cat_meow_wav) );
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop()
{
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}
