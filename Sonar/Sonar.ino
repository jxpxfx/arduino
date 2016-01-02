#include <LiquidCrystal.h>

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int pingPin = 5;
int inPin = 4;
long duration, inches, cm;
int indec, cmdec;
int inchconv = 147;
int cmconv = 59;
String s1, s2;

void setup() {
  Serial.begin(9600);
  //lcd.begin(16, 2);
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
}

void loop()
{
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);

  duration = pulseIn(inPin, HIGH);

  inches = microsecondsToInches(duration);
  indec = (duration - inches * inchconv) * 10 / inchconv;
  cm = microsecondsToCentimeters(duration);
  cmdec = (duration - cm * cmconv) * 10 / cmconv;
  s1 = String(inches) + "." + String(indec) + "in" + "     ";
  s2 = String(cm) + "." + String(cmdec) + "cm" + "     ";
//  lcd.setCursor(0, 0);
//  lcd.print(s1);
//  lcd.setCursor(0,1);
//  lcd.print(s2);
  //Send to the cloud
  //Debug
  Serial.println(s2);

  delay(500);
}

long microsecondsToInches(long microseconds)
{
  return microseconds / inchconv;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
}

