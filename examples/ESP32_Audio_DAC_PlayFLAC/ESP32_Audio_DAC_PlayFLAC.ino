#include <Arduino.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorFLAC.h>

#include "sample.h"

AudioOutputI2S *out;
AudioFileSourcePROGMEM *file;
AudioGeneratorFLAC *flac;

void setup()
{
  Serial.begin(115200);
  Serial.println("Play FLAC from PROGMEM...\n");

  audioLogger = &Serial;
  file = new AudioFileSourcePROGMEM( sample_flac, sizeof(sample_flac) );
  out = new AudioOutputI2S(0, 1); // port=0, output_mode = INTERNAL_DAC
  flac = new AudioGeneratorFLAC();
  flac->begin(file, out);
}

void loop()
{
  if (flac->isRunning()) {
    if (!flac->loop()) flac->stop();
  } else {
    Serial.printf("FLAC done\n");
    delay(1000);
  }
}
