/*
 * VHS Telephone
 * Created by Daniel Oulton
 *  for Outside the March
 *  "The Tape Escape"
 *  
 *  Usage:
 *  A keypad is used to dial a number that the audience member finds
 *  A corresponding MP3 is played.
 *  The MP3s are loaded from the SD card in the form "PHONENUMBER.MP3"
 *  Every time a key is pressed, the number is loaded to the stack. 
 *  When # is pressed, the number is "Dialed" ie, searched for on the SD card
 *  If found the mp3 is played, if not, play a busy signal.
 *  The * key is to pick up or hang up. This also clears the stack.
 *  
 *  Schematic:
 *                   Keypad
 *                      v
 *  5v USB power --> Arduino -> 5v stereo amp => Stereo speakers
 *                      v^            ^
 *                VS1053 MP3 Decoder--|
 *                      
 *  Debug:
 *  When booting, the device plays 2 tones to indicate the speakers
 *  and SD card are working.
 *  To debug, open the serial monitor at 115200 baud.
 *  The expected output is: VS1053 found, SD OK!
 *  Followed by: a printout of the files on the SD card 
 *  Every time a key is pressed, the stack is printed.
 *  As well, every time a number is dialed, the search results
 *  are displayed.
 */

 /* Included Libraries:
 * Keypad.h - https://github.com/Chris--A/Keypad
 * Adafruit_VS1053.h **Compiled with version 1.07** - https://github.com/adafruit/Adafruit_VS1053_Library
 *                   1.08 should work fine though
 */

#define VOLUME 40 //lower numbers = louder!

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

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 5, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte ledPin = 13; 

boolean blink = false;
boolean ledPin_state;

String s = "";
boolean dialing;

void setup(){
  Serial.begin(115200);

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  delay(500);
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  musicPlayer.setVolume(VOLUME,VOLUME);
  
  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate SD Card is working
  
  // list files
  printDirectory(SD.open("/"), 0);
 
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));

    pinMode(ledPin, OUTPUT);              // Sets the digital pin as output.
    digitalWrite(ledPin, HIGH);           // Turn the LED on.
    ledPin_state = digitalRead(ledPin);   // Store initial LED state. HIGH when LED is on.
    keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
}

void loop(){
    char key = keypad.getKey();

    if (key) {
        Serial.println(s);
        delay(100);
//        search2Play(s);
    }
    
    if (blink){
        digitalWrite(ledPin,!digitalRead(ledPin));    // Change the ledPin from Hi2Lo or Lo2Hi.
        delay(50);
    }
}

// Handle keypresses
// Todo: Fuction for hanging up/cancelling dialtone?
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
         switch (key){
          case '#': //hit # to dial
            search2Play(s);
            s = "";
            break;
          case '*': //hit * to bring up dialtone
            s = "";
            search2Play("DIALTONE");
            break;
          default: //otherwise, play the tone for the number dialed
            playTone(String(key));
            s += key;
            break;

    case RELEASED:
        break;

    case HOLD:
        if (key == '*') {
            //blink = true;    // Blink the LED when holding the * key.
            s = "";
        }
        break;
    }
    }
    }

int search2Play(String str){
Serial.print("entering search: ");
String fileName = str;
//if something's playing and we hit *, hang up and reset the stack
if(musicPlayer.playingMusic && str == "DIALTONE"){
  musicPlayer.stopPlaying();
}else{
fileName.concat(".MP3");
char c_fileName[fileName.length()+1];
fileName.toCharArray(c_fileName, fileName.length()+1);
Serial.println(c_fileName);
if(SD.exists(fileName)){
  Serial.println("Found File");
  if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
  }
  if (! musicPlayer.startPlayingFile(c_fileName)) {
  Serial.println("Error opening file");
  return 1;
  }
}else{
  search2Play("BUSY");
}
}
fileName = "";
return 0;
}

void playTone(String k)
{
String fileName = "DTMF-";
fileName.concat(k);
fileName.concat(".MP3");
char c_fileName[fileName.length()+1];
fileName.toCharArray(c_fileName, fileName.length()+1);
Serial.println(c_fileName);
if(SD.exists(fileName)){
  Serial.println("Found File");
  if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
  }
  if (! musicPlayer.startPlayingFile(c_fileName)) {
  Serial.println("Error opening file");
  }
}
fileName = "";
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

