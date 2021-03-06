//configuration.h file
//const char* ssid     = "ssid";
//const char* password = "pass";
//
//const char* host = "api.thingspeak.com";
//const char* channelId = "channel";

///ADXL345Esp8266Cloud//////////////////////////////////////////////////////
// ESP8266-01 & ADXL345 acceleration sensor
// in Arduino IDE
//
// Stefan Thesen 09/2015
//
// Free for anybody - no warranties
// code majorly taken from
//    https://www.sparkfun.com/tutorials/240
//    and adapted to ESP8266
/////////////////////////////////////////////////////////

#include "configuration.h"
#include "math.h"
#include <Wire.h>

#define DEVICE (0x53) // Device address as specified in data sheet

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
char DATAY0 = 0x34;    //Y-Axis Data 0
char DATAY1 = 0x35;    //Y-Axis Data 1
char DATAZ0 = 0x36;    //Z-Axis Data 0
char DATAZ1 = 0x37;    //Z-Axis Data 1

#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2 //GPIO2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String writeAPIKey = channelId;

float h;
float t;
float f;

int value = 0;
float angleX = 0.0;

//sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

void setup()
{
  setupAccel();
  dht.begin();
  readAccel();  
  Serial.println("start...");
  connectWifi();
  updateDweet();
  //Serial.println("goint to sleep. good night.");
  ESP.deepSleep(300*1000000, WAKE_RF_DEFAULT); // Sleep for 1 minutes
}

void loop()
{
  //readAccel();  // read the x/y/z tilt
  //delay(200);   // only read every 200ms
}

void setupAccel()
{
  pinMode (RED, OUTPUT);
  pinMode (GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  
  Serial.begin(9600);   // start serial for output. Make sure you set your Serial Monitor to the same!
  Serial.println("Init - S. Thesen ESP8266 & ADXL345 Demo");

  // i2c bus SDA = GPIO0; SCL = GPIO2
  Wire.begin(4, 5);

  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x01);

  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);
}

void readAccel()
{
  uint8_t howManyBytesToRead = 6;
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345

  // each axis reading comes in 10 bit resolution, ie 2 bytes. Least Significat Byte first!!
  // thus we are converting both bytes in to one
  // cave: esp8266 is 32bit, thus use short and not int as in Arduino examples
  short x = (((short)_buff[1]) << 8) | _buff[0];
  short y = (((short)_buff[3]) << 8) | _buff[2];
  short z = (((short)_buff[5]) << 8) | _buff[4];

  angleX = x * 2. / 512;
  Serial.print("x: ");
  //Serial.print( x * 2. / 512 );
  Serial.print( angleX );
  Serial.print(" y: ");
  Serial.print( y * 2. / 512 );
  Serial.print(" z: ");
  Serial.print( z * 2. / 512 );
  Serial.print("  Total: ");
  Serial.println( sqrtf(x * x + y * y + z * z) * 2. / 512 );
  
  if (angleX < 0.15)
  {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
  }
  else if (angleX > 0.40) 
  {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
  }
}

void writeTo(byte address, byte val)
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[])
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while (Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

void connectWifi(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network
  digitalWrite(RED, HIGH);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(RED, LOW);
  blinkLed(BLUE);

}

void updateDweet(){
  Serial.print("updateDweet method");
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    return;
  }

  // We now create a URI for the request
  sensorValue = analogRead(analogInPin);
  readSensorData();
  String tsData = "field1=";
  tsData += angleX;
  tsData += "&field2=";
  tsData += t;
  tsData += "&field3=";
  tsData += h;
  
  Serial.print("connected. angleX=");
  Serial.println(angleX);
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(tsData.length());
  client.print("\n\n");

  client.print(tsData);
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  blinkLed(GREEN);
  Serial.println("closing connection");
  delay(2000);
}

void blinkLed(int color) {
  Serial.println("will blink now...");
  for (int i = 0; i < 2; i++) {
    digitalWrite(color, HIGH);
    delay(100);
    digitalWrite(color, LOW);
    delay(50);
  }
}

void turnOff(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

void readSensorData() {
  do {
    Serial.println("Trying to read from DHT sensor!");
    delay(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    f = dht.readTemperature(true); 
  } while (isnan(h) || isnan(t) || isnan(f));

  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t) || isnan(f)) {
  //  Serial.println("Failed to read from DHT sensor!");
  //  return;
  //}

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
}

