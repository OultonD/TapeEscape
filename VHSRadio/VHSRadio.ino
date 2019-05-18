/*
 *  VHS Radio
 *  Created by Daniel Oulton
 *  for Outside the March
 *  "The Tape Escape"
 *  
 *  Usage:
 *  An LCD displays the frequency the "radio" is tuned to.
 *  An encoder is used to seek a station that the audience member
 *  has been told to listen to. 
 *  When the right station is selected, an MP3 is played.
 *  MP3s should be in the format [3-4 digits]AM.mp3 OR
 *  [3-4 digits]FM.mp3. Note that for FM stations, omit the
 *  decimal in the frequency.
 *  The "AM/FM" Switch also triggers a servo which rotates a car.
 *  
 *  Schematic:
 *                    Encoder + Switch
 *                          ^
 *  5v 2A Wall Wart -> Arduino Nano -> LCD
 *                          ^v
 *                    VS1053 MP3 Decoder -> 5v Stereo Amp => Stereo Speaker
 *  
 *  Debug:
 *  To debug, open the serial monitor at 115200 baud
 *  
 */
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

#define VOLUME 10

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <Encoder.h>
Encoder enc(2, A2);

#include <Button.h>
Button encButton(A1);
bool pwr = false;

#include <Servo.h>
Servo s;

#define SERVO_PIN 5
#define SERVO_START 0
#define SERVO_END 170 //in degrees

#define AM_FM 7 //AM/FM Switch

#define ENCBUTTON A1

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

unsigned long currMillis = 0;
unsigned long prevMillis = 0;
int freqDelay = 500; //how long, in ms, do we need to land on a freq before playing?
int lastFreq = 0; //what was the frequency, last time we checked?
bool freqFlag = false;


bool playingStatic = false;
bool lastSwitchReading = true;
bool isFM;

void setup() {
  Serial.begin(9600);
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 50);    // Make a tone to indicate VS1053 is working
  delay(50);
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  musicPlayer.setVolume(VOLUME,VOLUME);
  
  musicPlayer.sineTest(0x44, 50);    // Make a tone to indicate SD Card is working
  
  // list files
  printDirectory(SD.open("/"), 0);
 
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
  setupLCD();
  // put your setup code here, to run once:
  
  encButton.begin();
  
  pinMode(AM_FM, INPUT_PULLUP); //0 = AM, 1 = FM;

  isFM = digitalRead(AM_FM);
  lastSwitchReading = isFM;

  s.attach(SERVO_PIN);
  rotateCar();
  if(isFM){
    enc.write(fmFreq);
  }else{
    enc.write(amFreq);
  }

  //musicPlayer.stopPlaying();
  search2Play("BLANK"); //I literally don't know why this is needed but it is.
                      //I think the MP3 board needs to play something before the loop starts or it crashes.
  delay(1000);
  powerDown();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(pwr && encButton.pressed()){
    pwr = false;
  }
  if(!pwr){
    powerDown();
  }
  
  isFM = digitalRead(AM_FM);
//  if(playingStatic && !musicPlayer.playingMusic){
//    Serial.println(F("Looping static"));
//    playingStatic = false;
//    search2Play("STATIC");
//    playingStatic = true;
//    delay(100);
//  }
  readEnc();
  int freq = getFreq();

  if(isFM != lastSwitchReading){
    musicPlayer.stopPlaying();
    playingStatic = false;
    rotateCar();
  }
  
  if((freq != lastFreq) || (isFM != lastSwitchReading)){
  updateDisplay();
  currMillis = millis(); //what time did we land on this freq?
 if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
  }
  playingStatic = false;
  freqFlag = true; //let your freq flag fly
  }
  
  if(freq == lastFreq && (millis() - currMillis) >= freqDelay && freqFlag){
     search2Play(String(freq));
     freqFlag = false; //to prevent an infinite loop;
  }
  lastSwitchReading = isFM;
  lastFreq = freq;
  //Serial.println("Loop");
  //delay(50);
}

void updateDisplay(){
  lcd.home();
  lcd.clear();
  if(isFM){
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
Serial.print(F("entering search: "));
String fileName = str;
if(isFM){
  fileName.concat(F("FM.MP3"));
}else{
  fileName.concat(F("AM.MP3"));
}
char c_fileName[fileName.length()+1];
fileName.toCharArray(c_fileName, fileName.length()+1);
Serial.println(c_fileName);
if(SD.exists(c_fileName)){
  Serial.println("Found File");
  if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
  }
  if (! musicPlayer.startPlayingFile(c_fileName)) {
  Serial.println(F("Error opening file"));
  return 1;
  }
  playingStatic = false;
}else{
  Serial.println(F("File not found. Playing static"));
  if(!playingStatic){
  String radioStatic = "FUZZ";
  if(isFM){
    radioStatic.concat(fmFreq%10);
  }else{
    radioStatic.concat(amFreq/10%10);
  }
  search2Play(radioStatic);
  playingStatic = true;
  }else{
  Serial.println(F("Already playing static, skipping"));
  }
}
fileName = "";
return 0;
}

bool readEnc(){
  encCurrRead = enc.read()/2;
  if(encCurrRead == encLastRead){
    return 0;
  }
  if(isFM){ //FM dial
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
  delay(100);
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
    Serial.println(F(" - LCD found."));

  } else {
    Serial.println(F(" - LCD not found."));
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

int getFreq(){
  if(isFM){
    return fmFreq;
  }else{
    return amFreq;
  }
}

void rotateCar(){
  if(isFM){
    s.write(SERVO_START);
  }else{
    s.write(SERVO_END);
  }
}

void powerDown(){
  Serial.println(F("Powering down"));
  if(musicPlayer.playingMusic){
    musicPlayer.stopPlaying();
  }
  playingStatic = false;
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Goodbye.");
  delay(2500);
  lcd.clear();
  lcd.setBacklight(0);
  amFreq = AMMIN;
  fmFreq = FMMIN;
  bool pwrBtn = false;
  do{
    pwrBtn = encButton.pressed(); //wait for the power button to be pressed
  }while(!pwrBtn);
  Serial.println(F("Powering up"));
  lcd.setBacklight(255);
  delay(700);
  pwr = true;
}

//bool isFM()
//{
//  bool reading = digitalRead(AM_FM);
//  Serial.println(reading);
//  return digitalRead(reading);
//}
//
//bool switchChanged()
//{
//  bool reading = isFM();
//  if(lastSwitchReading == reading){
//    //Serial.println("Switch unchanged");
//    return 0;
//  }else{
//    lastSwitchReading = reading;
//    Serial.print("Switch changed to ");
//    Serial.println(reading);
//    return 1;
//  }
//}


