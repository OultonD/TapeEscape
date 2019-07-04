/*
 * Created by Daniel Oulton
 * for Outside the March
 * "The Tape Escape"
 *  
 * Usage:
 * The Arduino reads 5 digital inputs ever 50ms (adjustable)
 * and prints the output to the serial port at 9600 baud
 * The switches are activated by the presence of a magnet
 * 
 * Schematic:
 *                    |-------SW1-|
 *                    | |-----SW2-|
 *  5v USB Power -> Arduino --SW3-|
 *                    | |-----SW4-|
 *                    |-------SW5-|
 *                                |
 *                               GND
 */
#define SW_1 2
#define SW_2 A0
#define SW_3 A1
#define SW_4 A2
#define SW_5 A3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("On");
  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);
  pinMode(SW_3, INPUT_PULLUP);
  pinMode(SW_4, INPUT_PULLUP);
  pinMode(SW_5, INPUT_PULLUP);

}

void loop() {
Serial.print(!digitalRead(SW_1));
Serial.print(!digitalRead(SW_2));
Serial.print(!digitalRead(SW_3));
Serial.print(!digitalRead(SW_4));
Serial.print(!digitalRead(SW_5));
Serial.println();
delay(50);
}

