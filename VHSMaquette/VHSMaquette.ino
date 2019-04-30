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

#include <Servo.h>
#include <Button.h>

#define SW_1 A0
#define SW_2 A1
#define SW_3 A2
#define SW_4 A3
#define SW_5 2
#define SERVO_PIN 5
#define SERVO_START 0
#define SERVO_END 90 //in degrees

Button b1(SW_1);
Button b2(SW_2);
Button b3(SW_3);
Button b4(SW_4);
Button b5(SW_5);

bool result = true;
int answers[4];
int key[] = {1,2,3,4}; //answer key

Servo s;
int pos = 0;

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
  Serial.println("Servo Set");
  b1.begin();
  b2.begin();
  b3.begin();
  b4.begin();
  b5.begin();
}

void loop() {
  Serial.println("entered loop");
    s.write(SERVO_START);
  // put your main code here, to run repeatedly:
 for(int i=0; i<4; i++)
 {
   bool _b1 = false;
   bool _b2 = false;
   bool _b3 = false;
   bool _b4 = false;
   bool _b5 = false;
 
   do{
    _b1 = b1.pressed();
    _b2 = b2.pressed();
    _b3 = b3.pressed();
    _b4 = b4.pressed();
    _b5 = b5.pressed();
   }while(!_b1 && !_b2 && !_b3 && !_b4 && !_b5);
   
   if(_b1){
    answers[i] = 1;
   }
   if(_b2){
    answers[i] = 2;
   }
   if(_b3){
    answers[i] = 3;
   }
   if(_b4){
    answers[i] = 4;
   }
   if(_b5){
    answers[i] = 5;
   }
   Serial.println(answers[i]);
 }
 checkResult();
}

void checkResult()
{
  for(int i = 0; i<4; i++)
  {
    result = result && (answers[i] == key[i]);
  }
  Serial.println(result);
  if(result)
  {
    //play success file
    if (! musicPlayer.startPlayingFile("SUCCESS.MP3")) {
    Serial.println("Error opening file");
    }
    while(!b5.pressed())
    {
      delay(10);
    }
    openDoor();
    while(!b5.pressed())
    {
      delay(10);
    }
  }
  else
  {
    //play failure file
    if (! musicPlayer.startPlayingFile("FAILURE.MP3")) {
    Serial.println("Error opening file");
  }
  }
  
}

void openDoor()
{
  if (! musicPlayer.startPlayingFile("FFWD.MP3")) {
  Serial.println("Error opening file");
  }
  delay(6100);
  Serial.println("Moving servo");
  for(int i = SERVO_START; i<SERVO_END; i++)
  {
    s.write(i);
    delay(135);
  }
  Serial.println("done");
  delay(1000);
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

