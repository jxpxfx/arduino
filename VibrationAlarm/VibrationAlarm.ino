/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 2;
int max = 5000;
int count = 0;
int prevState = 999;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  //Serial.println(prevState);
  if(buttonState == 1)
  {    
    //if (prevState != buttonState)
    //{
      Serial.print(".");
    //}    
    prevState = buttonState;
    count = 0;
  }
  else
  {
    //Serial.println("0");
    count++;
  }

  //Serial.println(count);
  if (count == max)
  {
    Serial.println();
    Serial.println("---alarm---");
    prevState = buttonState;
    count = 0;
  }  
  delay(1);        // delay in between reads for stability
}



