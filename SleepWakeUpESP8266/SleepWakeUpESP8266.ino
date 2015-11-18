#include <Ticker.h> // Ticker can periodically call a function
Ticker blinker; // Ticker object called blinker.

int ledPin = 12; // LED is attached to ESP8266 pin 5.
uint8_t ledStatus = 0; // Flag to keep track of LED on/off status
int counter = 0; // Count how many times we've blinked

void setup() 
{
  pinMode(ledPin, OUTPUT); // Set LED pin (5) as an output
  blinker.attach(0.25, blink); // Ticker on blink function, call every 250ms
}

void loop() 
{

}

void blink()
{
  Serial.println("start of blink");
  if (ledStatus)
    digitalWrite(ledPin, HIGH);
  else
    digitalWrite(ledPin, LOW);
  ledStatus = (ledStatus + 1) % 2; // Flip ledStatus

  if (counter++ > 20) {
    // If we've blinked 20 times
    Serial.println("going deep sleep");
    ESP.deepSleep(10000000, WAKE_RF_DEFAULT); // Sleep for 10 seconds
    Serial.println("after deep sleed");
    counter = 0;
  }    
  Serial.println("end of blink");
}
