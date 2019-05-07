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
bool isFM = false;

#define AMMAX 1700
#define AMMIN 540
#define AMSTEP 10
int amFreq = AMMIN;

#define FMMAX 1079
#define FMMIN 881 
#define FMSTEP 2 //in hundreds of khz
int fmFreq = FMMIN;

void setup() {
  // put your setup code here, to run once:
  pinMode(AM_FM, INPUT_PULLUP); //0 = AM, 1 = FM;
  isFM = digitalRead(AM_FM);
}

void loop() {
  // put your main code here, to run repeatedly:

}
