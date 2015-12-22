#include <Wire.h>

#define accel_module (0x53)
byte values[6] ;
char output[512];

const int VERTICAL = 13;

void setup(){
  pinMode(VERTICAL, OUTPUT);
  digitalWrite(VERTICAL, LOW);
Wire.begin(4,5);
Serial.begin(9600);
Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(0);
Wire.endTransmission();
Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(16);
Wire.endTransmission();
Wire.beginTransmission(accel_module);
Wire.write(0x2D);
Wire.write(8);
Wire.endTransmission();
}

void loop(){
int xyzregister = 0x32;
int x, y, z;

Wire.beginTransmission(accel_module);
Wire.write(xyzregister);
Wire.endTransmission();

Wire.beginTransmission(accel_module);
Wire.requestFrom(accel_module, 6);

int i = 0;
while(Wire.available()){
values[i] = Wire.read();
i++;
}
Serial.print(values[0]);
Serial.print(",");
Serial.print(values[1]);
Serial.print(",");
Serial.print(values[2]);
Serial.print(",");
Serial.print(values[3]);
Serial.print(",");
Serial.print(values[4]);
Serial.print(",");
Serial.println(values[5]);
Wire.endTransmission();
if (values[0] >= 200 && values[0] <= 240)
{
    digitalWrite(VERTICAL, HIGH);
} else {
    digitalWrite(VERTICAL, LOW);
}
delay(100);
}
