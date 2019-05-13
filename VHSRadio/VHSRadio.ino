/* Included 3rd Party Libraries:
 * Button.h - https://github.com/madleech/Button
 * LiquidCrystal_PCF8574.h - https://github.com/mathertel/LiquidCrystal_PCF8574
 * Encoder.h - https://github.com/PaulStoffregen/Encoder
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

#define VOLUME 40

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <Encoder.h>
Encoder enc(2, A2);

#include <Servo.h>

#define SERVO_PIN 5
#define SERVO_START 0
#define SERVO_END 90 //in degrees

#define AM_FM 7 //AM/FM Switch

#define AMMAX 1700
#define AMMIN 540
#define AMSTEP 10
int amFreq = AMMIN;

#define FMMAX 1079
#define FMMIN 881 
#define FMSTEP 2 //in hundreds of khz
int fmFreq = FMMIN;

int encLastRead = 1;
int encCurrRead = 1;

bool playingStatic = false;

void setup() {
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
  setupLCD();
  // put your setup code here, to run once:
  pinMode(AM_FM, INPUT_PULLUP); //0 = AM, 1 = FM;
  
  if(isFM()){
    enc.write(fmFreq);
  }else{
    enc.write(amFreq);
  }

  //musicPlayer.stopPlaying();
  search2Play("BUSY"); //I literally don't know why this is needed but it is.
}

void loop() {
  // put your main code here, to run repeatedly:
  if(playingStatic && !musicPlayer.playingMusic){
    playingStatic = false;
    search2Play("STATIC");
  }
  
  if(readEnc()){
  updateDisplay();
  if(isFM()){
    search2Play(String(fmFreq));
  }else{
    search2Play(String(amFreq));
  }
  }
  //Serial.println("Loop");
  //delay(50);
}

void updateDisplay(){
  lcd.home();
  lcd.clear();
  if(isFM()){
    if(fmFreq >999){
      lcd.setCursor(1,0);
    }else{
      lcd.setCursor(2,0);
    }
    float freq = fmFreq/10.0;
    String s = String(freq,1);
    s += "FM";
    lcd.print(s);
  }else{
    if(amFreq > 999){
      lcd.setCursor(1,0);
    }else{
      lcd.setCursor(2,0);
    }
    String s = String(amFreq);
    s += "AM";
    lcd.print(s);
  }
}

int search2Play(String str){
Serial.print("entering search: ");
String fileName = str;
if(isFM){
  fileName.concat("FM");
}else{
  fileName.concat("AM");
}
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
  playingStatic = false;
}else{
  if(!playingStatic){
  search2Play("STATIC");
  playingStatic = true;
  }else{
  Serial.println("Already playing static, skipping");
  }
}
fileName = "";
return 0;
}

bool readEnc(){
  encCurrRead = enc.read();
  if(encCurrRead == encLastRead){
    return 0;
  }
  if(isFM()){ //FM dial
    if(encCurrRead > encLastRead){
      fmFreq += FMSTEP;
    }else if(encCurrRead < encLastRead){
      fmFreq -= FMSTEP;
    }
    if(fmFreq < FMMIN){
      fmFreq = FMMAX;
    }
    if(fmFreq > FMMAX){
      fmFreq = FMMIN;
    }
    Serial.println(fmFreq);
  }else{      //AM dial
    if(encCurrRead > encLastRead){
      amFreq += AMSTEP;
    }else if(encCurrRead < encLastRead){
      amFreq -= AMSTEP;
    }
    if(amFreq < AMMIN){
      amFreq = AMMAX;
    }
    if(amFreq > AMMAX){
      amFreq = AMMIN;
    }
    Serial.println(amFreq);
  }
  encLastRead = encCurrRead;
  return 1;
}

void setupLCD()
{
    // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x27);
  int error = Wire.endTransmission();
  Serial.print(error);

  if (error == 0) {
    Serial.println(" - LCD found.");

  } else {
    Serial.println(" - LCD not found.");
  } 

  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  lcd.home(); lcd.clear(); lcd.noCursor();
  lcd.print("Hello.");
  delay(250);
  lcd.clear();
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

bool isFM()
{
  return digitalRead(AM_FM);
}

