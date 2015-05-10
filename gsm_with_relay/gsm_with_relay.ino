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
 // what to do next?
 Serial.write(buffer, count);
 Serial.println("Called parseBuffer");
 printPhoneNumber();
 int seconds = timeForPhoneToCharge();
 Serial.println("seconds: " + String(seconds));
 if (seconds == -1) return;
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

int timeForPhoneToCharge() {
  String message(buffer);
  if (!message.startsWith("+CMT")) return -1;
  int i;
  for (i = 0; i < count && buffer[i] != '\n'; i++) {
    Serial.write(buffer[i]);
  }
  if (buffer[i] != '\n') return -1;  // no newline found.
  if (i == count) return -2;  // end of string.
  Serial.println(message.substring(i+1));
  return message.substring(i+1).toInt();
}

void printMessage() {
  String message(buffer);
  if (!message.startsWith("+CMT")) return;
  // Parse message
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
