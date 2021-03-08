
#include "sound.h"
#include "XT_DAC_Audio.h"

                                      
XT_DAC_Audio_Class DacAudio(25,0);          // Create the main player class object. Use GPIO 25, one of the 2 DAC pins and timer 0

XT_Wav_Class Sound0(SoundData_0);
XT_Wav_Class Sound1(SoundData_1);
XT_Wav_Class Sound2(SoundData_2);
XT_Wav_Class Sound3(SoundData_3);
XT_Wav_Class Sound4(SoundData_4);
XT_Wav_Class Sound5(SoundData_5);
XT_Wav_Class Sound6(SoundData_6);
XT_Wav_Class Sound7(SoundData_7);
XT_Wav_Class Sound8(SoundData_8);
XT_Wav_Class Sound9(SoundData_9);
XT_Wav_Class Sound10(SoundData_10);
XT_Wav_Class Sound11(SoundData_11);
XT_Wav_Class Sound12(SoundData_12);
XT_Wav_Class Sound13(SoundData_13);
XT_Wav_Class Sound14(SoundData_14);
XT_Wav_Class Sound15(SoundData_15);
XT_Wav_Class Sound16(SoundData_16);

XT_Sequence_Class Sequence;               // The sequence object, you add your sounds above to this object (see setup below)


void setup() {
  Serial.begin(115200);
}

void loop() {
  DacAudio.FillBuffer();          // This needs only be in your main loop once, suggest here at the top.

  // Get a number entered from the user on the serial port
  if(Serial.available()) 
    PlayNumber(Serial.readString().c_str());
}

void PlayNumber(char const *Number)
{  
  int NumChars=strlen(Number);              // could lose this line of put strlen in loop below, but bad form to do so
  Sequence.RemoveAllPlayItems();            // Clear out any previous playlist
  for(int i=0;i<NumChars;i++)
    AddNumberToSequence(Number[i]);         // For each number add in the sound for that number to the sequence
  DacAudio.Play(&Sequence);                 // Play the sequence, will not wait here to complete, works independently of your code
  Serial.println(Number);                   // Confirm number entered to the user over the serial
}

void AddNumberToSequence(char TheNumber)
{
  // Adds in the wav for the single 0-9, a-f number passed in as a char

  switch(TheNumber)
  {
    case '0' : Sequence.AddPlayItem(&Sound0);break;
    case '1' : Sequence.AddPlayItem(&Sound1);break;
    case '2' : Sequence.AddPlayItem(&Sound2);break;
    case '3' : Sequence.AddPlayItem(&Sound3);break;
    case '4' : Sequence.AddPlayItem(&Sound4);break;
    case '5' : Sequence.AddPlayItem(&Sound5);break;
    case '6' : Sequence.AddPlayItem(&Sound6);break;
    case '7' : Sequence.AddPlayItem(&Sound7);break;
    case '8' : Sequence.AddPlayItem(&Sound8);break;
    case '9' : Sequence.AddPlayItem(&Sound9);break;
    case 'a' : Sequence.AddPlayItem(&Sound10);break;
    case 'b' : Sequence.AddPlayItem(&Sound11);break;
    case 'c' : Sequence.AddPlayItem(&Sound12);break; 
    case 'd' : Sequence.AddPlayItem(&Sound13);break;
    case 'e' : Sequence.AddPlayItem(&Sound14);break;
    case 'f' : Sequence.AddPlayItem(&Sound15);break; 
    case 'g' : Sequence.AddPlayItem(&Sound16);break;                   
  }
}
