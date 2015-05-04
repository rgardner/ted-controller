/* Relay Test
 *
 * Toggle the load on the corresponding relay pin.
 */
int relayPin1 = 2;  // IN1 connected to corresponding pin on arduino

void setup() {
  pinMode(relayPin1, OUTPUT);     // sets the digital pin as output
  digitalWrite(relayPin1, HIGH);  // Prevents relays from starting up engaged
}

void loop() {
  digitalWrite(relayPin1, LOW);   // energizes the relay and lights the LED
  delay(1000);                    // waits for a second
  digitalWrite(relayPin1, HIGH);  // de-energizes the relay and LED is off
  delay(1000);                    // waits for a second
}
