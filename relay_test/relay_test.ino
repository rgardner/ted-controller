/* Relay Test
 *
 * Toggle the load on the corresponding relay pin.
 */
const int relayPin = 22;
const int currentSensorPin = 8;
const int speakerPin = 12;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);    // sets the digital pin as output
  digitalWrite(relayPin, LOW);  // Prevents relays from starting up engaged
  pinMode(currentSensorPin, INPUT);  // initialize current sensor
  pinMode(speakerPin, OUTPUT);  // activate speaker for debugging
}

void loop() {
  Serial.println(analogRead(currentSensorPin));
  delay(1000);

  // digitalWrite(relayPin, LOW);  // energizes the relay and lights the LED
  // tone(speakerPin, 440, 1000);
  // delay(5000);

  // digitalWrite(relayPin, HIGH);  // de-energizes the relay and LED is off
  // delay(1000);
}
