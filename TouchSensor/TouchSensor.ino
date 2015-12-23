/*
  DigitalReadSerial
 Reads a digital input on pin 9, prints the result to the serial monitor

 This example code is in the public domain.
 */

// digital pin 9 has a pushbutton attached to it. Give it a name:
int pushButton = 9;
const int LED = 17; //that's for pro micro - uses the RX LED

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  Serial.println(buttonState);
  if (buttonState == 0)
  {
    digitalWrite(LED, HIGH);
  }
  else 
  {
    digitalWrite(LED, LOW);
  }
  delay(1);        // delay in between reads for stability
}



