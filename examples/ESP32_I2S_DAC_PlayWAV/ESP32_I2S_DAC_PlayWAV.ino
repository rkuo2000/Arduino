// I2S DAC: MAX98357A
// Library: ESP8266Audio 
// port=0, output_mode = 0 (EXTERNAL_I2S)
// bclkPin = 26;  wclkPin = 25;  doutPin = 22;

#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "SoundData.h"

AudioFileSourcePROGMEM *file;
AudioGeneratorWAV *wav;
AudioOutputI2S *out;

void setup()
{
  Serial.begin(115200);
  Serial.println("WAV start\n");
  audioLogger = &Serial;
  file = new AudioFileSourcePROGMEM( SoundData, sizeof(SoundData) );
  out = new AudioOutputI2S();                                           
  wav = new AudioGeneratorWAV();
  wav->begin(file, out); 
}

void loop()
{
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");    
    // regenerate WAV
    file = new AudioFileSourcePROGMEM( SoundData, sizeof(SoundData) );
    out = new AudioOutputI2S();                                           
    wav = new AudioGeneratorWAV();
    wav->begin(file, out);
  }
}
