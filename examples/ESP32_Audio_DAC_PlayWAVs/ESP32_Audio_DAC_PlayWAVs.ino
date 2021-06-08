#include <Arduino.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>

#include "sound.h"

AudioOutputI2S *out;
AudioFileSourcePROGMEM *file;
AudioGeneratorWAV *wav;

void setup()
{
  Serial.begin(115200);
  Serial.println("Play WAVs from PROGMEM...\n");

  audioLogger = &Serial;
  //file= new AudioFileSourcePROGMEM( SoundData, sizeof(SoundData) );
  out = new AudioOutputI2S(0, 1); // port=0, output_mode = INTERNAL_DAC
  wav = new AudioGeneratorWAV();
  //wav->begin(file, out);
}

void loop() 
{
  char c;
  if(Serial.available()) {
    c = Serial.read();
    Serial.print("Play SoundData ");
    Serial.println(c);
  }

  switch(c){
    case '0': file = new AudioFileSourcePROGMEM( SoundData_0, sizeof(SoundData_0) ); break;
    case '1': file = new AudioFileSourcePROGMEM( SoundData_1, sizeof(SoundData_1) ); break;
    case '2': file = new AudioFileSourcePROGMEM( SoundData_2, sizeof(SoundData_2) ); break;
    case '3': file = new AudioFileSourcePROGMEM( SoundData_3, sizeof(SoundData_3) ); break;
    case '4': file = new AudioFileSourcePROGMEM( SoundData_4, sizeof(SoundData_4) ); break;
    case '5': file = new AudioFileSourcePROGMEM( SoundData_5, sizeof(SoundData_5) ); break;
    case '6': file = new AudioFileSourcePROGMEM( SoundData_6, sizeof(SoundData_6) ); break;
    case '7': file = new AudioFileSourcePROGMEM( SoundData_7, sizeof(SoundData_7) ); break;
    case '8': file = new AudioFileSourcePROGMEM( SoundData_8, sizeof(SoundData_8) ); break;
    case '9': file = new AudioFileSourcePROGMEM( SoundData_9, sizeof(SoundData_9) ); break;
    case 'a': file = new AudioFileSourcePROGMEM( SoundData_10, sizeof(SoundData_10) ); break;
    case 'b': file = new AudioFileSourcePROGMEM( SoundData_11, sizeof(SoundData_11) ); break;
    case 'c': file = new AudioFileSourcePROGMEM( SoundData_12, sizeof(SoundData_12) ); break;
    case 'd': file = new AudioFileSourcePROGMEM( SoundData_13, sizeof(SoundData_13) ); break;
    case 'e': file = new AudioFileSourcePROGMEM( SoundData_14, sizeof(SoundData_14) ); break;
    case 'f': file = new AudioFileSourcePROGMEM( SoundData_15, sizeof(SoundData_15) ); break;        
    case 'g': file = new AudioFileSourcePROGMEM( SoundData_16, sizeof(SoundData_16) ); break;
  }

  if ((c>=0x30 && c<=0x39) || (c>=0x61 && c<=0x67))
    wav->begin(file, out);
  
  while (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      Serial.println("WAV done");
    }
  }

  delay(1000);
}
