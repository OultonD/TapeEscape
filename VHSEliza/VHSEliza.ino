/*
 *  VHS Eliza Machine
 *  Reminder: All questions must fit within a 16x2 matrix
 *        ie:
 *    ################
 *    ################
 * 
 */


/* Included 3rd Party Libraries:
 * Button.h - https://github.com/madleech/Button
 * LiquidCrystal_PCF8574.h - https://github.com/mathertel/LiquidCrystal_PCF8574
 * Encoder.h - https://github.com/PaulStoffregen/Encoder
 */


#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <Encoder.h>
Encoder enc(2, A2);

#include <Button.h>

Button trueButton(A1);
Button falseButton(A0);
Button encButton(A3);

//#define trueButton A1
//#define falseButton A0
#define trueLED 6
#define falseLED 5

unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
int fadeInterval = 10;
int fadeIncrement = 1;
bool fadeDir = true; //true for up, false for down
byte brightness = 0;

#include "Path.h"
int path = -1;
const int maxPaths = 8;
bool responses[4];

Path pathList[maxPaths] = Path();

void setup() {
  Serial.begin(115200);
  Serial.println("LCD...");

  trueButton.begin();
  falseButton.begin();
  encButton.begin();
  pinMode(trueLED, OUTPUT);
  pinMode(falseLED, OUTPUT);

  while (! Serial);

  setQuestions();
  setupLCD();
  ledON();
  enc.write(1);

}

void loop() {

  // put your main code here, to run repeatedly:
  if(path == -1)
  {
    choosePath();
  }
  else
  {
    lcd.home();
    lcd.clear();
    lcd.print("Press any button");
    lcd.setCursor(0,1);
    lcd.print("to begin.");
    while(!trueButton.pressed() && !falseButton.pressed())
    {
      pulse();
    }
      ledON();
    ask(pathList[path-1]);
    giveResult(path-1);
    while(!encButton.pressed())
    {
      pulse();
    }
    ledOFF();
    path = -1;
  }
}

void choosePath()
{
  int newPath;
  int curPath = path;
  while(!encButton.pressed())
  {
   newPath = enc.read()/2; 
   if(newPath != curPath)
   {
       if(newPath > maxPaths)
       {
        newPath = maxPaths;
        enc.write(maxPaths*2); //compensate for 2 ISR per clock
       }
       if(newPath <= 1)
       {
        enc.write(1*2);
       }
       newPath = enc.read()/2;
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Select Path");
       lcd.setCursor(0,1);
       lcd.print(newPath);
   }
   curPath = newPath;
  }
  path = newPath;
}

void ask(Path p)
{
  int numQ = p.getNumQuestions();
  Serial.print("Asking ");
  Serial.println(path);
  for(int i = 0; i<numQ; i++)
  {
    String L0 = "Question " + String(i+1);
    String L1 = "True or Lies";
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(L0);
  lcd.setCursor(2,1);
  lcd.print(L1);
  bool tB = false;
  bool fB = false;
  Serial.println("Printed lines?");
  Serial.println(L0);
  Serial.println(L1);
  do{
    tB = trueButton.pressed();
    fB = falseButton.pressed();
  }while(!tB && !fB);
 
  if(tB)
  {
    responses[i] = true;
    Serial.println("T");
  }
  else
  {
    responses[i] = false;
    Serial.println("F");
  }

  }
}

//void setQuestions()
//{
//  for(int i = 0; i<maxPaths; i++)
//  {
//    for(int j = 0; j<4; j++)
//    {
//      String q = "Question " + String(j);
//      pathList[i].setQuestion(j,q,"");
//    } 
//  }
//}

void setQuestions() // ZERO INDEXED, SORRY
{
  Serial.println("Setting Questions");
//  pathList[0].setQuestion(0, "Question 1:", "True or False?");
//  pathList[0].setQuestion(1, "Question 2:", "True or False?");
//  pathList[0].setQuestion(2, "Question 3:", "True or False?");
//  pathList[0].setQuestion(3, "Question 4:", "True or False?");
  pathList[0].setCorrectResult("Grmmys Gr8st Vol2");
  pathList[0].setIncorrectResult("Amelie");
  bool key0[] = {false, false, true, true};
  pathList[0].setAnswerKey(key0);
  
//  pathList[1].setQuestion(0, "Question 1:", "True or False?");
//  pathList[1].setQuestion(1, "Question 2:", "True or False?");
//  pathList[1].setQuestion(2, "Question 3:", "True or False?");
//  pathList[1].setQuestion(3, "Question 4:", "True or False?");
  pathList[1].setCorrectResult("Super Mario III");
  pathList[1].setIncorrectResult("Raging Bull");
  bool key1[] = {true, false, true, false};
  pathList[1].setAnswerKey(key1);
//
//  pathList[2].setQuestion(0, "Question 1:", "True or False?");
//  pathList[2].setQuestion(1, "Question 2:", "True or False?");
//  pathList[2].setQuestion(2, "Question 3:", "True or False?");
//  pathList[2].setQuestion(3, "Question 4:", "True or False?");
  pathList[2].setCorrectResult("Sleepless in Seattle");
  pathList[2].setIncorrectResult("Paddle to the Sea");
  bool key2[] = {false, false, true, true};
  pathList[2].setAnswerKey(key2);

//  pathList[3].setQuestion(0, "Question 1:", "True or False?");
//  pathList[3].setQuestion(1, "Question 2:", "True or False?");
//  pathList[3].setQuestion(2, "Question 3:", "True or False?");
//  pathList[3].setQuestion(3, "Question 4:", "True or False?");
  pathList[3].setCorrectResult("Thelma and Louise");
  pathList[3].setIncorrectResult("Monsoon Wedding");
  bool key3[] = {false, true, true, false};
  pathList[3].setAnswerKey(key3);

//  pathList[4].setQuestion(0, "Question 1:", "True or False?");
//  pathList[4].setQuestion(1, "Question 2:", "True or False?");
//  pathList[4].setQuestion(2, "Question 3:", "True or False?");
//  pathList[4].setQuestion(3, "Question 4:", "True or False?");
  pathList[4].setCorrectResult("Billy Madison");
  pathList[4].setIncorrectResult("Joseph and the Technicolour Dream Coat");
  bool key4[] = {false, true, false, false};
  pathList[4].setAnswerKey(key4);

//  pathList[5].setQuestion(0, "Question 1:", "True or False?");
//  pathList[5].setQuestion(1, "Question 2:", "True or False?");
//  pathList[5].setQuestion(2, "Question 3:", "True or False?");
//  pathList[5].setQuestion(3, "Question 4:", "True or False?");
  pathList[5].setCorrectResult("Jack the Bear");
  pathList[5].setIncorrectResult("Splitting Heirs");
  bool key5[] = {false, true, true, false};
  pathList[5].setAnswerKey(key5);
  
//  pathList[6].setQuestion(0, "Question 1:", "True or False?");
//  pathList[6].setQuestion(1, "Question 2:", "True or False?");
//  pathList[6].setQuestion(2, "Question 3:", "True or False?");
//  pathList[6].setQuestion(3, "Question 4:", "True or False?");
  pathList[6].setCorrectResult("Dr. No");
  pathList[6].setIncorrectResult("Feathers for Felines");
  bool key6[] = {true, true, false, false};
  pathList[6].setAnswerKey(key6);

//  pathList[7].setQuestion(0, "Question 1:", "True or False?");
//  pathList[7].setQuestion(1, "Question 2:", "True or False?");
//  pathList[7].setQuestion(2, "Question 3:", "True or False?");
//  pathList[7].setQuestion(3, "Question 4:", "True or False?");
  pathList[7].setCorrectResult("Godzilla 1986");
  pathList[7].setIncorrectResult("Dead Poets Society");
  bool key7[] = {true, true, false, false};
  pathList[7].setAnswerKey(key7);
  Serial.println("Done");
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


void giveResult(int p)
{
  lcd.home();
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Thinking...");
  for(int i = 0; i<2; i++)
  {
    for(int j = 0; j<3; j++)
    {
      lcd.scrollDisplayLeft();
      delay(350);
    }
    for(int k = 0; k<5; k++)
    {
      lcd.scrollDisplayRight();
      delay(350);
    }
    for(int l = 0; l<2; l++)
    {
      lcd.scrollDisplayLeft();
      delay(350);
    }
  }
  lcd.home();
  lcd.clear();
  for(int i = 0; i<4; i++)
  {
    Serial.print(responses[i]);
    Serial.print(" ");
  }
  Serial.println();
  if(pathList[p].checkResult(responses))
  {
        String result = pathList[p].getCorrectResult();
        int rLen = result.length();
        if(rLen > 16){
          for(int i = 0; i<3; i++){ //number of times to scroll
            lcd.setCursor(3,0); //Centre text
            lcd.print("They rent:");
            lcd.setCursor(0,1); //second line
            lcd.print(result);
            delay(1000);
            for(int j = 0; j<(rLen-16); j++){ //scroll remaining characters
              lcd.scrollDisplayLeft();
              delay(300);
            }
            delay(1000);
            lcd.clear();
            delay(300);
            lcd.setCursor(0,1);
          }
          lcd.print(result);
        }else{
        lcd.setCursor(3,0); //Centre text
        lcd.print("They rent:");
        lcd.setCursor(0,1); //second line
        lcd.print(result);
        }
  }
  else //incorrect case
  {
        String result = pathList[p].getIncorrectResult();
        int rLen = result.length();
        if(rLen > 16){
          for(int i = 0; i<3; i++){ //number of times to scroll
            lcd.setCursor(3,0); //Centre text
            lcd.print("They rent:");
            lcd.setCursor(0,1); //second line
            lcd.print(result);
            delay(1000);
            for(int j = 0; j<(rLen-16); j++){ //scroll remaining characters
              lcd.scrollDisplayLeft();
              delay(300);
            }
            delay(1000);
            lcd.clear();
            delay(300);
            lcd.setCursor(0,1);
          }
          lcd.print(result);
        }else{
        lcd.setCursor(3,0); //Centre text
        lcd.print("They rent:");
        lcd.setCursor(0,1); //second line
        lcd.print(result);
        }
  }
}

void pulse()
{
  currentMillis = millis();
  if(currentMillis-prevMillis > fadeInterval)
  {
    if(brightness + fadeIncrement > 255)
    {
      fadeDir = false;
    }
    if(brightness - fadeIncrement < 0)
    {
      fadeDir = true;
    }
    
    if(fadeDir)
    {
      brightness += fadeIncrement;
    }
    else
    {
      brightness -= fadeIncrement;
    }
prevMillis = currentMillis;
  }
analogWrite(trueLED, brightness);
analogWrite(falseLED, brightness);
}

void ledON()
{
  digitalWrite(trueLED, 1);
  digitalWrite(falseLED, 1);
  brightness = 255;
}

void ledOFF()
{
  digitalWrite(trueLED, 0);
  digitalWrite(falseLED, 0);
  brightness = 0;
}
