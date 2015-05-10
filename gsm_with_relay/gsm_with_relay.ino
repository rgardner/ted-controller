#include <SoftwareSerial.h>
#include <String.h>
#include <Time.h>
#include <TimeAlarms.h>

/*******************************************************************
 * Power Sensing */
const int NUM_SAMPLES = 1000;  // number of analog read samples
const int SAMPLE_RATE = 1;     // number of seconds in between samples
const int currentPin1 = 8;     // analog pin
int currentData1[NUM_SAMPLES];
int currentDataCount = 0;
int nextSampleTime = 0;  // stores the time the last sample was taken

/*******************************************************************
 * Relays */
const int NO_AVAILABLE_RELAYS = -1;
const int NUM_RELAYS = 1;
int relays[NUM_RELAYS];
const int RELAY_PIN_START = 22;

/*******************************************************************
 * SMS */
String PARSE_ERROR = "";
SoftwareSerial gprsSerial(10, 11);
char buffer[64];  // buffer array for data receive over serial port
int count = 0;    // counter for buffer array


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

  if (now() > nextSampleTime) {
    currentData1[currentDataCount++] = analogRead(currentPin1);
    nextSampleTime += SAMPLE_RATE;
    if (currentDataCount >= NUM_SAMPLES) currentDataCount = 0;
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
  Serial.println(response);
  delay(100);
  // Ignore responses that do not contain text messages.
  if (response.indexOf("CMT") == -1) return;
  Serial.println("response is a CMT command.");

  String sender = getResponseSender(response);
  String message = getResponseMessage(response);
  Serial.println("sender: " + sender);
  Serial.println("message: '" + message + "'");
  if (sender == PARSE_ERROR || message == PARSE_ERROR) {
    Serial.println("ERROR: could not parse response.");
    return;
  }

  int seconds = stringToInt(message);
  Serial.println("seconds: " + String(seconds));
  if (seconds == NULL) {
    char errorMessage[] = "Cannot parse your message. Please respond with " \
                          "the time in seconds you'd like to charge your " \
                          "phone. e.g. '10', '30', '60', etc.";
    sendTextMessage(errorMessage, sender);
    return;
  }
  int error = startPoweringPhone(seconds);
  if (error == NO_AVAILABLE_RELAYS) {
    char errorMessage[] = "No available relays found. Please try again later.";
    sendTextMessage(errorMessage, sender);
    return;
  }
  Serial.println("Exiting parseBuffer");
}


/*******************************************************************
 * Power Sensing */
void printPowerData() {
  Serial.println(currentDataCount);
  for (int i = 0; i < currentDataCount; i++) {
    Serial.println(currentData1[i]);
  }
}

/*******************************************************************
 * Relay Functions */
int startPoweringPhone(int seconds) {
  int i = 0;
  for ( ; i < NUM_RELAYS; i++) {
    if (relays[i] == -1) break;
  }
  if (i == NUM_RELAYS) return NO_AVAILABLE_RELAYS;
  relays[i] = now() + seconds;
  const int pin = RELAY_PIN_START + i;
  digitalWrite(pin, LOW);
  Alarm.timerOnce(seconds, powerPhoneOff);
  return 0;
}

void powerPhoneOff() {
  for (int i = 0; i < NUM_RELAYS; i++) {
    if (relays[i] == -1) continue;
    if (now() > relays[i]) {
      int pin = RELAY_PIN_START + i;
      Serial.println("Powering off pin: " + String(pin));
      digitalWrite(pin, HIGH);
      relays[i] = -1;
      printPowerData();
    }
  }
}


/*******************************************************************
 * SMS Functions */
// Send 'message' to 'toPhoneNumber'
boolean sendTextMessage(String message, String toPhoneNumber) {
  Serial.println("Sending Text...");
  gprsSerial.println("AT+CMGS = \"" + toPhoneNumber + "\"");
  delay(100);
  gprsSerial.println(message); //the content of the message
  delay(100);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
}

// Returns the sender's phone number. Return empty string ("") if unable to
// parse.
String getResponseSender(String response) {
  String phoneNumber = "";
  int startPhoneNumber = response.indexOf("\"");
  if (startPhoneNumber == -1) return PARSE_ERROR;
  int endPhoneNumber = response.indexOf("\"", startPhoneNumber + 1);
  if (endPhoneNumber == -1) return PARSE_ERROR;
  return response.substring(startPhoneNumber + 1, endPhoneNumber);
}

// TODO: remove after getResponseSender is functional.
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

// Returns PARSE_ERROR if the message cannot be parsed (no newline found).
String getResponseMessage(String response) {
  int newlinePos = response.indexOf('\n');
  if (newlinePos == -1) return PARSE_ERROR;
  int finalNewlinePos = response.indexOf('\n', newlinePos+1);
  if (finalNewlinePos == -1) return response.substring(newlinePos + 1);
  return response.substring(newlinePos+1, finalNewlinePos);
}


/*******************************************************************
 * Miscellaneous */
// Returns NULL if input contains a non-digit character.
int stringToInt(String input) {
  // XXX(rgardner): the following loop breaks the parsing of the message. This
  //   should just check that every character is in fact a digit before
  //   parsing it as an integer. However, there is a digit that is not a
  //   number even when all newlines have been removed.
  /*for (int i = 0; i < input.length(); i++) {*/
    /*if (!isDigit(input.charAt(i))) return NULL;*/
  /*}*/
  return input.toInt();
}
