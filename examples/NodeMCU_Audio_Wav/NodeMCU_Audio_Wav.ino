//
// NodeMCU play wave from PROGMEM
//
// NodeMCU connections:
// Vin (5V)      to Speaker+
//      Speaker- to 2N3904-Collector          
// RX (I2Sout)   to 2N3904-Base
// Gnd           to 2N3904-Emitter

// Notes:
// I2Sout is sharing UART-RX pin, so do not connect to Speaker circuit till code is uploaded
// wav->stop will close "file", so need to reopen file to replay the wav file.

#include <Arduino.h>
#include <ESP8266WiFi.h>

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
    delay(3000); // delay 3000ms = 3s 

    // wav->stop will close the file, so need to reopen the file to replay wav file      
    file = new AudioFileSourcePROGMEM( cat_meow_wav, sizeof(cat_meow_wav) );
    out = new AudioOutputI2SNoDAC();
    wav = new AudioGeneratorWAV();
    wav->begin(file, out);      
  }
}
