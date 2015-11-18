/*
This is a simple example on how to blink the RGB led on the ESP12-E

 based on:

 ESP8266 Blink by Simon Peter
 Blink the blue LED on the ESP-01 module
 This example code is in the public domain
 
 The blue LED on the ESP-01 module is connected to GPIO1 
 (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 
 Note that this sketch uses BUILTIN_LED to find the pin with the internal LED
*/

int r = 12;
int g = 13;
int b = 15;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(r, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(500);                      // Wait for a second
  digitalWrite(r, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(g, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(b, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(500);                      // Wait for two seconds (to demonstrate the active low LED)
  digitalWrite(g, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(500);                      // Wait for a second
  digitalWrite(r, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(g, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(b, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(500);                      // Wait for two seconds (to demonstrate the active low LED)
  digitalWrite(b, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(500);                      // Wait for a second
  digitalWrite(r, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(g, HIGH);  // Turn the LED off by making the voltage HIGH
  digitalWrite(b, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(500);                      // Wait for two seconds (to demonstrate the active low LED)
}
