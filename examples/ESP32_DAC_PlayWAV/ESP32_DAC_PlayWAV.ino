/*
	Game_Audio Library
	
	This library is based heavily on the XT_DAC_Audio libray
	http://www.xtronical.com/basics/audio/digitised-speech-sound-esp32-playing-wavs/
	
	This library is designed for 8-bit quality audio that an old school video 
	game might use. This leaves plenty of cpu resources for the video game logic.

	The interrupt is sync'd with the sample rate. This removes the need to do any 
	complex math in the interrupt. Using the FPU in the interrupt appears to have been
	causing some instability and crashes when I loaded up the CPU with game logic.
	
	(c) B. Dring 2018, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty given
	
*/

#include "ForceBeWithYou.h"
#include "Game_Audio.h"

Game_Audio_Class GameAudio(25,0); 

Game_Audio_Wav_Class pmWav(SoundData);                            
                                      

void setup() {
  Serial.begin(115200); 
  Serial.println("Begin...");
}

void loop() {   
	Serial.print("Sample Rate (Hz):");
	Serial.println(pmWav.getSampleRate());
	Serial.print("Duration (secs):");
	Serial.println(pmWav.getDuration());
	
    GameAudio.PlayWav(&pmWav, false, 1.0);
	
	// wait until done
	while(GameAudio.IsPlaying()){		
	}
	
}
