const int pinSwitch = 5; //Pin Reed

const int pinLed = 13; //Pin LED

int StatoSwitch = 0;

void setup()

{

pinMode(pinLed, OUTPUT); //Imposto i PIN

pinMode(pinSwitch, INPUT);

}

void loop()

{

StatoSwitch = digitalRead(pinSwitch); //Leggo il valore del Reed

if (StatoSwitch == HIGH)

{
Serial.println("HIGH");
digitalWrite(pinLed, HIGH);

}

else

{
Serial.println("LOW");
digitalWrite(pinLed, LOW);

}

}
