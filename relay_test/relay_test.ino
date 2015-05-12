/* Relay Test
 *
 * Toggle the load on the corresponding relay pin.
 */
int relayPin1 = 22;  // IN1 connected to corresponding pin on arduino

void setup() {
  Serial.begin(9600);
  pinMode(relayPin1, OUTPUT);     // sets the digital pin as output
  digitalWrite(relayPin1, LOW);  // Prevents relays from starting up engaged
  pinMode(12, OUTPUT);
}

void loop() {
  int data = analogRead(8);
  Serial.println(data);
  delay(1000);
  // digitalWrite(relayPin1, LOW);   // energizes the relay and lights the LED
  // tone(12, 440, 1000);
  // delay(5000);                    // waits for a second
  
  // digitalWrite(relayPin1, HIGH);  // de-energizes the relay and LED is off
  // delay(1000);                    // waits for a second
}
