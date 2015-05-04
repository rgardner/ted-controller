/*
  TED Controller

  Mediates interactions between the customer, the backend payment system, and
  the battery. Customers create accounts and then authenticate them here on
  the controller.

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
// The number of accounts to store.
const byte HASH_SIZE = 10;
// Storage: where the accounts will live in memory.
HashType<char*, char*> hashRawArray[HASH_SIZE];
// Handles the storage [search, retrieve, insert]
HashMap<char*, char*> hashMap = HashMap<char*, char*>(hashRawArray, HASH_SIZE);

/*************************************************************************
 * Miscellaneous */
// The input buffer.
int buffer_index = 0;
char buffer[1000];

// The current account being authenticated.
bool has_account = false;
char account_number[1000];

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a message to the LCD.
  welcome();

  // seed some accounts.
  hashMap[0](4256149938, 1234);
  hashMap[1](3107289332, 0000);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0)
  lcd.setCursor(col, 1);

  char key = keypad.getKey();

  // Could not read key.
  if (!key) return;
  
  // This marks the end of user input.
  if (key == '#') {
    if (!has_account) {
      // We need to verify that the account exists.
      if (hashMap.getValueOf(buffer) == NULL) {
        // Print error message.
        lcd.clear();
        lcd.print("Incorrect account number, please try again");
        delay(1000);
        lcd.clear();
        lcd.print("Account Number:");

        // Clear the buffer.
        memset(buffer, 0, sizeof(buffer));
        col = 0;
        return;
      }
      lcd.clear();
      lcd.print("Success! Found account.");
      has_account = true;

      // Copy buffer into account and clear the buffer.
      strncpy(buffer, account_number, buffer_index);
      memset(buffer, 0, sizeof(buffer);

      // Reset and print new instructions
      lcd.print("Enter your password now:");
      col = 0;
    } else {
      // Check to see if this password is in the system.
      char *password = hashMap.getValueOf(account_number);
      if (!strncmp(buffer, password, buffer_index)) {
        lcd.clear();
        lcd.print("Incorrect password!");
        lcd.setCursor(0, 1);
        lcd.print("Please try again");
        memset(buffer, 0, sizeof(buffer));
        col = 0;
        return; 
      }
      lcd.clear();
      lcd.home();
      lcd.print("Success!");
      // TODO(rgardner): enter in payment information code.
      //   It will cost you $X to charge your phone for X
      }
    }
  }
  
  // Still entering input.
  if (has_account) {
    // Hide password.
    lcd.print("#");
  } else {
    lcd.print(key);
  }
  col++;
  // Wrap the screen when the user enters more than 16 characters.
  col %= 16;

  // Add character to the buffer.
  buffer[buffer_index] = key;
  buffer_index++;
}

void welcome() {
  lcd.clear();
  lcd.home();
  lcd.print("Welcome to TED!");
  lcd.setCursor(0, 1);
  lcd.print("Have an account? Enter your cell phone number now.");
  delay(1500);
  lcd.clear();
  lcd.home();
  lcd.print("Account Number:");
}
