#include <Arduino.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>

#include "SoundData.h"

AudioOutputI2S *out;
AudioFileSourcePROGMEM *file;
AudioGeneratorWAV *wav;

void setup()
{
  Serial.begin(115200);
  Serial.println("Play WAV from PROGMEM...\n");

  audioLogger = &Serial;
  file = new AudioFileSourcePROGMEM( SoundData, sizeof(SoundData) );
  out = new AudioOutputI2S(0, 1); // port=0, output_mode = INTERNAL_DAC
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
