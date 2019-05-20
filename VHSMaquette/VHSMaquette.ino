 /*
 *  VHS Maquette
 *  Created by Daniel Oulton
 *  for Outside the March
 *  "The Tape Escape"
 *  
 *  Usage:
 *  Audience members press 4 buttons in order to hear a clue mp3 (SUCCESS.MP3)
 *  They then search for 4 buttons (Play, RWD, FFW, Eject). Each button plays a sound.
 *  Comment out "#define ONE_CHANCE" to allow multiple pushes to play the sound over again
 *  Fast forward also flashes the "Shop LED". Eject opens up a hatch via servo.
 *  
 *  Schematic:
 *              4x tree buttons  
 *                  v
 *  5v 2A PSU -> Arduino Nano -> VS1053 MP3 Player -> Audio isolator -> 5v amplifier -> Speakers
 *                  ^       |--> LED
 *                  |       |--> Servo
 *               4x VCR buttons
 *  Debug:
 *  To debug, open the serial monitor at 9600 baud
 *  
 *  Required Files on the SD Card:
 *  The MP3 playing functions require the files to be named as such:
 *  ONE.MP3 - Played when tree button 1 is pressed
 *  TWO.MP3 -   "     "     "   "     2  "  "
 *  THREE.MP3 - Tree button 3
 *  FOUR.MP3 -  Tree button 4
 *  SUCCESS.MP3 - Played when the right Tree-quence is pressed
 *  FAILURE.MP3 - Played when the WRONG Tree-quence is pressed
 *  
 *  PLAY.MP3    - When the PLAY button is pressed
 *  REWIND.MP3  - When REWIND is pressed
 *  FFWD.MP3    - When FAST FORWARD is pressed
 *  EJECT.MP3   - When EJECT is pressed
 * 
 */
 
 /* Included 3rd party Libraries:
 * Keypad.h - https://github.com/Chris--A/Keypad
 * Adafruit_VS1053.h **Compiled with version 1.07** - https://github.com/adafruit/Adafruit_VS1053_Library
 *                   1.08 should work fine though
 */

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

#include <Keypad.h>

const byte ROWS = 2; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3','4'},
    {'P','R','F','E'} //PLAY, RWD, FFW, EJECT
};

byte rowPins[ROWS] = {A0, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, A3, A2, A1}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#include <Servo.h>

#define SERVO_PIN 5
#define SERVO_START 0
#define SERVO_END 90 //in degrees

#define LED 6

bool result = true;
int answers[] = {0,0,0,0};
int key[] = {1,2,3,4}; //answer key

Servo s;
int pos = 0;

//#define ONE_CHANCE //this is the trigger that determines if the play/rwd etc buttons can be pushed more than once. 
                   //Comment it out to allow multiple presses

bool played = false;
bool rewound = false;
bool fastforwarded = false;
bool ejected = false;

bool readyToReset = false; //if the eject button has been pushed, this flag is set

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(40,40);

  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
  

  s.attach(SERVO_PIN);
  s.write(SERVO_START);
  Serial.println("Servo Set");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  keypad.addEventListener(keypadEvent);
}

void loop() {
    char key = keypad.getKey();
    //Serial.println(key);
}


#ifdef ONE_CHANCE
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
         switch (key){
          case '1':
            shiftAnswers();
            answers[3] = 1;
            easyPlay("ONE.MP3");
            checkResult();
            break;
          case '2':
            shiftAnswers();
            answers[3] = 2;
            easyPlay("TWO.MP3");
            checkResult();
            break;            
          case '3':
            shiftAnswers();
            answers[3] = 3;
            easyPlay("THREE.MP3");
            checkResult();
            break;
          case '4':
            shiftAnswers();
            answers[3] = 4;
            easyPlay("FOUR.MP3");
            checkResult();
            break;
          case 'P':
            if(!played){
              played = true;
            PLAY();
            }else{
              Serial.println("Already played");
            }
            break;
          case 'R':
            if(!rewound){
            rewound = true;
            REWIND();              
            }else{
              Serial.println("Already rewound");
            }
            break;
          case 'F':
            if(!fastforwarded){
              FASTFORWARD();
              fastforwarded = true; //still not sure if this is a word
            }else{
              Serial.println("Already ffwd");
            }
            break;
          case 'E':
            if(!ejected){
             EJECT();   
             ejected = true;
            }else{
              Serial.println("Already ejected");
            }
            break;
          default:
            break;
         }  

    case RELEASED:
        break;

    case HOLD:
        switch(key){
          case '1':
            if(readyToReset){
            resetMaquette();
            }
            break;
        }
        break;

  }
}
#endif

#ifndef ONE_CHANCE
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
         switch (key){
          case '1':
            shiftAnswers();
            answers[3] = 1;
            easyPlay("ONE.MP3");
            checkResult();
            break;
          case '2':
            shiftAnswers();
            answers[3] = 2;
            easyPlay("TWO.MP3");
            checkResult();
            break;            
          case '3':
            shiftAnswers();
            answers[3] = 3;
            easyPlay("THREE.MP3");
            checkResult();
            break;
          case '4':
            shiftAnswers();
            answers[3] = 4;
            easyPlay("FOUR.MP3");
            checkResult();
            break;
          case 'P':
            PLAY();
            break;
          case 'R':
            REWIND();
            break;
          case 'F':
            FASTFORWARD();
            break;
          case 'E':
            EJECT();
            break;
          default:
            break;
         }  

    case RELEASED:
        break;

    case HOLD:
        switch(key){
          case '1':
            if(readyToReset){
            resetMaquette();
            }
            break;
        }
        break;

  }
}
#endif


void shiftAnswers() 
{
  answers[0] = answers[1]; //to enable a rolling check of the last 4 keypresses
  answers[1] = answers[2]; //shift the last 3 presses left by 1.
  answers[2] = answers[3];
  answers[3] = 0;
}

void checkResult()
{
  for(int i = 0; i<4; i++){
    Serial.print(answers[i]);
  }
  Serial.println();
  result = true;
  for(int i = 0; i<4; i++)
  {
    result = result && (answers[i] == key[i]);
  }
  Serial.println(result);
  if(result)
  {
      //play success file
      Serial.println("Success");
      easyPlay("SUCCESS.MP3");
  }
  else
  {
    //Play a sound if the wrong tree-quence was pressed
    //comment this out if you want a rolling stack of presses
    if(answers[0] != 0)
    {
         if(musicPlayer.playingMusic){
          musicPlayer.stopPlaying();
         }          
        if (! musicPlayer.startPlayingFile("FAILURE.MP3")) {
          Serial.println("Error opening file");
        }
        answers[0] = 0;
        answers[1] = 0;
        answers[2] = 0;
        answers[3] = 0;
    }
    
  }
}

void easyPlay(String fileName){
char c_fileName[fileName.length()+1];
fileName.toCharArray(c_fileName, fileName.length()+1);
  if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
   }    
  if (! musicPlayer.startPlayingFile(c_fileName)) {
    Serial.println(F("Error opening file"));
  }
}
  
void PLAY()
{
  Serial.println("Play");
  easyPlay("PLAY.MP3");
}

void REWIND()
{
  Serial.println(F("Rewind"));
  easyPlay(F("REWIND.MP3"));
}

void FASTFORWARD()
{
  Serial.println(F("Fast Forward"));
  easyPlay(F("FFWD.MP3"));
  for(int i = 0; i<3; i++){
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
}

void EJECT()
{
  Serial.println(F("Eject"));
  easyPlay(F("EJECT.MP3"));
  s.write(SERVO_END);
  readyToReset = true;
}

void resetMaquette(){
  Serial.println("Resetting");
  s.write(SERVO_START);
  readyToReset = false;
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

