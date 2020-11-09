// nRF24L01 Libraries
#include <SPI.h>
#include <RF24.h>

// Audio libraries
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;  // six memory players, so we can play
AudioPlayMemory    sound2;  // all six sounds simultaneously
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioPlayMemory    sound5;
AudioMixer4        mix1;    // two 4-channel mixers are needed in
AudioMixer4        mix2;    // tandem to combine 6 audio sources
AudioOutputI2S     headphones;
AudioOutputAnalog  dac;     // play to both I2S audio board and on-chip DAC

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mix1, 0);
AudioConnection c2(sound1, 0, mix1, 1);
AudioConnection c3(sound2, 0, mix1, 2);
AudioConnection c4(sound3, 0, mix1, 3);
AudioConnection c5(mix1, 0, mix2, 0);   // output of mix1 into 1st input on mix2
AudioConnection c6(sound4, 0, mix2, 1);
AudioConnection c7(sound5, 0, mix2, 2);
AudioConnection c8(mix2, 0, headphones, 0);
AudioConnection c9(mix2, 0, headphones, 1);
AudioConnection c10(mix2, 0, dac, 0);

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;
int songNum = 0;



RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";
boolean crossed = 0; 

void setup() {

 
  Serial.begin(9600);
    // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // turn on the output
  audioShield.enable();
  audioShield.volume(0.5);

  // by default the Teensy 3.1 DAC uses 3.3Vp-p output
  // if your 3.3V power has noise, switching to the
  // internal 1.2V reference can give you a clean signal
  //dac.analogReference(INTERNAL);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  mix1.gain(0, 0.4);
  mix1.gain(1, 0.4);
  mix1.gain(2, 0.4);
  mix1.gain(3, 0.4);
  mix2.gain(1, 0.4);
  mix2.gain(2, 0.4);
  
  Serial.begin(115200);
  while (!Serial);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setChannel( 90 );
  radio.openReadingPipe(0, address);
  radio.printDetails();
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println( "Ready" );
}
void loop() {
  if (radio.available()) {
  //char text[32] = "";
  //radio.read(&text, sizeof(text));
  radio.read(&crossed, sizeof(crossed));
  Serial.print( "[" );
  Serial.print( crossed);
  Serial.println( "]" );

  if (crossed == 1) {

  switch (songNum) { 
    
     case 0: {
       sound0.play(AudioSampleSnare);
       // statements
       break;
        }
    
    case 1: { 
      sound1.play(AudioSampleTomtom);
       // statements
       break;
        }
        
    case 2: { 
      sound2.play(AudioSampleHihat);
       // statements
       break;
        }
    case 3: { 
      sound3.play(AudioSampleKick);
       // statements
       break;
        }

    case 4: { 
      sound4.play(AudioSampleGong);
       // statements
       break;
        }

    case 5: { 
      sound5.play(AudioSampleCashregister);
       // statements
       break;
        }

    default: { 
       // statements
       break;
        }

    }  // End switch case

    songNum += 1;
    if (songNum == 5) songNum = 0;
    
  } // End if crossed == 1 

  } // End if radio available
  
} // End void loop()
