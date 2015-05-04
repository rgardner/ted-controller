/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include "Arduino.h"
#include "Keypad.h"
#include <HashMap.h>
#include <LiquidCrystal.h>

/*************************************************************************
 * Keypad globals */
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {13, 6, 5, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 2, 1}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int col = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


/*************************************************************************
 * Account globals */
const byte HASH_SIZE = 10;
HashType<int,int> hashRawArray[HASH_SIZE];
HashMap<int,int> hashMap = HashMap<int,int>(hashRawArray , HASH_SIZE);
char buffer[1000];

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  
  // seed some accounts.
  hashMap[0](4256149938, 1234);
  hashMap[1](3107289332, 0000);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(col, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);

  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      
    } else {
      lcd.print(key);
      col++;
      col %= 16;
    }
  }
}
