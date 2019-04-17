/*
 *  VHS Eliza Machine
 *  Reminder: All questions must fit within a 16x2 matrix
 *        ie:
 *    ################
 *    ################
 * 
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
int fadeInterval = 100;
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
  for(int i = 0; i<numQ; i++)
  {
  lcd.clear();
  lcd.home();
  lcd.print(p.getQuestionLine0(i));
  lcd.setCursor(0,1);
  lcd.print(p.getQuestionLine1(i));
  bool tB = false;
  bool fB = false;
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

void setQuestions() // ZERO INDEXED, SORRY
{

  pathList[0].setQuestion(0, "Question 1:", "True or False?");
  pathList[0].setQuestion(1, "Question 2:", "True or False?");
  pathList[0].setQuestion(2, "Question 3:", "True or False?");
  pathList[0].setQuestion(3, "Question 4:", "True or False?");
  pathList[0].setCorrectResult("Correct!");
  pathList[0].setIncorrectResult("Incorrect");
  bool key0[] = {true, true, true, true};
  pathList[0].setAnswerKey(key0);
  
  pathList[1].setQuestion(0, "Question 1:", "True or False?");
  pathList[1].setQuestion(1, "Question 2:", "True or False?");
  pathList[1].setQuestion(2, "Question 3:", "True or False?");
  pathList[1].setQuestion(3, "Question 4:", "True or False?");
  pathList[1].setCorrectResult("Correct!");
  pathList[1].setIncorrectResult("Incorrect");
  bool key1[] = {true, true, true, true};
  pathList[1].setAnswerKey(key1);

  pathList[2].setQuestion(0, "Question 1:", "True or False?");
  pathList[2].setQuestion(1, "Question 2:", "True or False?");
  pathList[2].setQuestion(2, "Question 3:", "True or False?");
  pathList[2].setQuestion(3, "Question 4:", "True or False?");
  pathList[2].setCorrectResult("Correct!");
  pathList[2].setIncorrectResult("Incorrect");
  bool key2[] = {true, true, true, true};
  pathList[2].setAnswerKey(key2);

  pathList[3].setQuestion(0, "Question 1:", "True or False?");
  pathList[3].setQuestion(1, "Question 2:", "True or False?");
  pathList[3].setQuestion(2, "Question 3:", "True or False?");
  pathList[3].setQuestion(3, "Question 4:", "True or False?");
  pathList[3].setCorrectResult("Correct!");
  pathList[3].setIncorrectResult("Incorrect");
  bool key3[] = {true, true, true, true};
  pathList[3].setAnswerKey(key3);

  pathList[4].setQuestion(0, "Question 1:", "True or False?");
  pathList[4].setQuestion(1, "Question 2:", "True or False?");
  pathList[4].setQuestion(2, "Question 3:", "True or False?");
  pathList[4].setQuestion(3, "Question 4:", "True or False?");
  pathList[4].setCorrectResult("Correct!");
  pathList[4].setIncorrectResult("Incorrect");
  bool key4[] = {true, true, true, true};
  pathList[4].setAnswerKey(key4);

  pathList[5].setQuestion(0, "Question 1:", "True or False?");
  pathList[5].setQuestion(1, "Question 2:", "True or False?");
  pathList[5].setQuestion(2, "Question 3:", "True or False?");
  pathList[5].setQuestion(3, "Question 4:", "True or False?");
  pathList[5].setCorrectResult("Correct!");
  pathList[5].setIncorrectResult("Incorrect");
  bool key5[] = {true, true, true, true};
  pathList[5].setAnswerKey(key5);
  
  pathList[6].setQuestion(0, "Question 1:", "True or False?");
  pathList[6].setQuestion(1, "Question 2:", "True or False?");
  pathList[6].setQuestion(2, "Question 3:", "True or False?");
  pathList[6].setQuestion(3, "Question 4:", "True or False?");
  pathList[6].setCorrectResult("Correct!");
  pathList[6].setIncorrectResult("Incorrect");
  bool key6[] = {true, true, true, true};
  pathList[6].setAnswerKey(key6);

  pathList[7].setQuestion(0, "Question 1:", "True or False?");
  pathList[7].setQuestion(1, "Question 2:", "True or False?");
  pathList[7].setQuestion(2, "Question 3:", "True or False?");
  pathList[7].setQuestion(3, "Question 4:", "True or False?");
  pathList[7].setCorrectResult("Correct!");
  pathList[7].setIncorrectResult("Incorrect");
  bool key7[] = {true, true, true, true};
  pathList[7].setAnswerKey(key7);
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
    lcd.print(pathList[p].getCorrectResult());
  }
  else
  {
    lcd.print(pathList[p].getIncorrectResult());
  }
}

void pulse()
{
  currentMillis = millis();
  if(currentMillis-prevMillis > fadeInterval)
  {
    if(brightness + fadeIncrement > 255)
    {
      fadeDir != fadeDir;
    }
    if(brightness - fadeIncrement < 0)
    {
      fadeDir != fadeDir;
    }
    
    if(fadeDir)
    {
      brightness += fadeIncrement;
    }
    else
    {
      brightness -= fadeIncrement;
    }
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

