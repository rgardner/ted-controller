/**
 * 1. Enter phone number.
 * 2. Open the Serial monitor with "No line ending" and "19200 baud"
 * 3. enter 't' into the input buffer and click "Send"
 * 4.
 */

#include <SoftwareSerial.h>
#include <String.h>

// +15408985543
String toPhoneNumber = "+14256149938";
SoftwareSerial gprsSerial(10, 11);
 
void setup()
{
  gprsSerial.begin(19200); // GPRS shield baud rate 
  Serial.begin(19200);
  delay(500);
}
 
void loop()
{
 
  if (Serial.available()) // if there is incoming serial data
   switch(Serial.read()) // read the character
   {
     case 't': // if the character is 't'
       SendTextMessage(); // send the text message
       break;
   } 
 
  if (gprsSerial.available()){ // if the shield has something to say
    Serial.write(gprsSerial.read()); // display the output of the shield
  }
}
 
/*
* Name: SendTextMessage
* Description: Send a text message to a number
*/
void SendTextMessage()
{
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  gprsSerial.println("AT+CMGS = \"" + toPhoneNumber + "\"");
  delay(100);
  gprsSerial.println("How are you today?"); //the content of the message
  delay(100);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
}
