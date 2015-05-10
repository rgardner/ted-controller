#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(10, 11);

char buffer[64];  // buffer array for data receive over serial port
int count = 0;  // counter for buffer array

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
  Serial.println("Ready...");
}

void loop() {
  //If a character comes in from the cellular module...
  if (gprsSerial.available() >0) {
    while(gprsSerial.available()) {
      buffer[count++]=gprsSerial.read();
      if (count == 64) break;
    }
    Serial.write(buffer, count);
    printPhoneNumber();
    clearBufferArray();
    count = 0;
  }
}

void clearBufferArray() {
  for (int i=0; i<count;i++) {
    buffer[i] = NULL;
  }
}

void printPhoneNumber() {
  /*Serial.write(buffer, count);*/
  /*Serial.println("Called printPhoneNumber");*/
  String message(buffer);
  /*Serial.println(message.substring(20));*/
  if (!message.startsWith("+CMT")) return;
  Serial.println("Entering function...");

  String phoneNumber = "";
  int i = 8;
  while (true) {
    /*Serial.println(buffer[i]);*/
    if (i == 64) break;
    if (buffer[i] == '"') break;
    phoneNumber.concat(buffer[i]);
    i++;
  }
  Serial.println(phoneNumber);
  Serial.println("Exiting function...");
}
