#include <Servo.h>

#define SW_1 3
#define SW_2 4
#define SW_3 5
#define SW_4 6 
#define SW_5 7
#define SERVO_PIN 2
#define SERVO_START 0
#define SERVO_END 180 //in degrees

const byte switchMask = B11111000;
byte result = 0;

Servo s;
int pos = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  s.attach(SERVO_PIN);
  DDRD = DDRD & ~switchMask; //set switches to input
  PORTD = PORTD | switchMask; //toggle pullups
}

void loop() {
  // put your main code here, to run repeatedly:
 result = ~PIND & switchMask;
 Serial.println(result, BIN);
 if(result == switchMask)
 {
  s.write(SERVO_END); //move the servo to open destination
 }
 else
 {
  s.write(SERVO_START);
 }
}

