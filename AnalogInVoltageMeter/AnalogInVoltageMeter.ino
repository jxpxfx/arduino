const int analogInPin = A0;
float sensorValue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(analogInPin);
  Serial.print("Voltage Output = ");
  Serial.print(sensorValue*5/1023);
  Serial.println(" ");
  delay(100);
}
