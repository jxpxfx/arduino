#define RELAY  8

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY , OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
   digitalWrite(RELAY,LOW);           // Turns ON Relays 1
   delay(2000);                                      // Wait 2 seconds
   digitalWrite(RELAY,HIGH);
   delay(2000);   
}
