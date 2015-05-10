#include <SoftwareSerial.h>
#include <String.h>
#include <Time.h>
#include <TimeAlarms.h>

/*******************************************************************
 * Power Sensing */
const int currentPin1 = 8;  // analog pin

/*******************************************************************
 * SMS */
SoftwareSerial gprsSerial(10, 11);

char buffer[64];  // buffer array for data receive over serial port
int count = 0;  // counter for buffer array

/*******************************************************************
 * Relays */
const int NUM_RELAYS = 1;
int relays[NUM_RELAYS];
const int RELAY_PIN_START = 22;

void setup()
{
  gprsSerial.begin(19200); // GPRS shield baud rate
  Serial.begin(19200);
  delay(100);
  // Read an SMS in index #1
  gprsSerial.print("AT+CMGF=1\r");
  delay(100);
  // Echo the contents of a new SMS upon receipt to serial out on GSM shield.
  gprsSerial.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);

  // Start pins and power them off.
  for (int i = 0; i < NUM_RELAYS; i++) {
    relays[i] = -1;
    const int pin = RELAY_PIN_START + i;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }

  // Start the power sensing pins.
  pinMode(currentPin1, INPUT);
  Serial.println("Ready...");
}

void loop() {
  //If a character comes in from the cellular module...
  if (gprsSerial.available() > 0) {
    while(gprsSerial.available()) {
      buffer[count++]=gprsSerial.read();
      if (count == 64) break;
    }
    parseBuffer();
    clearBufferArray();
    count = 0;
  }
  powerPhoneOff();
}

void clearBufferArray() {
  for (int i=0; i<count;i++) {
    buffer[i] = NULL;
  }
}

void parseBuffer() {
  /*Serial.println("Called parseBuffer");*/
  /*Serial.write(buffer, count);*/
  String response(buffer);
  // Ignore responses that do not contain text messages.
  if (!response.startsWith("+CMT")) return;

  String message = getResponseMessage(response);
  int seconds = stringToInt(message);
  Serial.println("seconds: " + String(seconds));
  if (seconds == NULL) return;
  startPoweringPhone(seconds);
  Serial.println("Exiting parseBuffer");
}

void startPoweringPhone(int seconds) {
  int i = 0;
  for ( ; i < NUM_RELAYS; i++) {
    if (relays[i] == -1) break;
  }
  if (i == NUM_RELAYS) {
    // ERROR, NO OPEN INPUT FOUND.
    return;
  }
  relays[i] = now() + seconds;
  const int pin = RELAY_PIN_START + i;
  digitalWrite(pin, LOW);
  Alarm.timerOnce(seconds, powerPhoneOff);
}

void printPhoneNumber() {
  String message(buffer);
  if (!message.startsWith("+CMT")) return;

  String phoneNumber = "";
  for (int i = 8; i < 65; i++) {
    if (buffer[i] == '"') break;
    phoneNumber.concat(buffer[i]);
  }
  Serial.println(phoneNumber);
}

// Returns "" (empty string) if the message cannot be parsed (no newline
//   found).
String getResponseMessage(String response) {
  int newlinePos = response.indexOf('\n');
  if (newlinePos == -1) return "";
  String message = response.substring(newlinePos + 1);
  Serial.println(message);
  return message;
}

// Returns NULL if input contains a non-digit character.
int stringToInt(String input) {
  for (int i = 0; i < input.length(); i++) {
    if (!isDigit(input.charAt(i))) return NULL;
  }
  return input.toInt();
}

void powerPhoneOff() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    if (relays[i] == -1) continue;
    if (now() > relays[i]) {
      int pin = RELAY_PIN_START + i;
      Serial.println("Powering off pin: " + String(pin));
      digitalWrite(pin, HIGH);
      relays[i] = -1;
    }
  }
}
