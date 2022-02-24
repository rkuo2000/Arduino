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
	
	Usage:
		1. Create an uncompressed, unsigned, mono, 8-bit wav file (the sample rate should be 2000 to 50000 Hz) 
		2. Use HxD to create a C array of the bytes.
		3. Place that in your program or an included header file.
		4. In your program create a GameAudioClass object. like ...
				 Game_Audio_Class GameAudio(25,0); 
					The first parameter is the DAC pin (25 or 26)
					The second parameter is the timer number to use
				 
		5. In your program create one or more GameAudioWavClass object(s) like ...
				 Game_Audio_Wav_Class pmWav(pacman);  
					The parameter is the array name you created in step 2.
		6. Play the wave file like this...
				GameAudio.PlayWav(&WavFile, false);
					The first parameter is a GameAudioWavClass object from step 5
					The second parameter determines if this wav will interrupt a currently playing one.
						If a wavefile is playing the function will return 0 and not play the requested wav
		
	Notes:
		1. If you try to play another wavefile before the first one ends
		   it will stop the first one and start the next one.
		   
		2. To wait for wav file to end, use the IsPlaying function of the GameAudio object.
		3. To stop a wav file from playing use the .StopPlaying function of the GameAudio object
		   
	
*/


/*
Here are the data Positions in the wav file specification.

1 - 4 "RIFF"  Marks the file as a riff file. Characters are each 1 byte long.
5 - 8 File size (integer) Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you'd fill this in after creation.
9 -12 "WAVE"  File Type Header. For our purposes, it always equals "WAVE".
13-16 "fmt "  Format chunk marker. Includes trailing null
17-20 Length of format data as listed above
21-22 1 Type of format (1 is PCM)
23-24 Number of Channels
25-28 Sample Rate - Number of Samples per second, or Hertz.
29-32 
33-34 
35-36 Bits per sample
37-40 "data"  "data" chunk header. Marks the beginning of the data section.
41-44 File size (data)  Size of the data section.


*/

#define ESP32_F_CPU 				80000000  // the speed of the processor
#define AUDIO_INTERRUPT_PRESCALER 	80     
#define MAX_SAMPLERATE 50000
#define MIN_SAMPLERATE  2000


// sample rate data locations ... see above
#define WAV_SAMPLERATE_L 24
#define WAV_SAMPLERATE_H 25

// PCM data size location ... see above 
#define WAV_FILESIZE_L 40 // LSB
#define WAV_FILESIZE_M 41
#define WAV_FILESIZE_H 42 // MSB

#define AUDIO_DATA_START 44 // this is where the data starts


// The Main Wave class for sound samples
class Game_Audio_Wav_Class
{
  public:      
  uint16_t SampleRate;  
  volatile uint32_t DataSize=0;                         
  volatile uint32_t DataIdx=AUDIO_DATA_START;
  volatile unsigned char *Data;  
  volatile bool Completed=true;
  
  Game_Audio_Wav_Class(unsigned char *WavData);
  float getDuration();  // return duration in seconds
  uint16_t getSampleRate();
};


class Game_Audio_Class
{
  // the main class for using the DAC to play sounds
  public:  
  volatile uint8_t DacPin;                              // pin to send DAC data to use 25 or 26 only!
  Game_Audio_Wav_Class *CurrentWav=0;                   // Wav to play
  
  Game_Audio_Class(uint8_t DacPin, uint8_t TimerNo);  
  int PlayWav(Game_Audio_Wav_Class *Wav, bool interruptCurrent, float sampleRateMultiplier);  
  void StopPlaying();
  int IsPlaying();
};

                                                          
