//configuration.h file
//const char* ssid     = "ssid";
//const char* password = "pass";
//
//const char* host = "api.thingspeak.com";
//const char* channelId = "channel";


#include "configuration.h"
#include "math.h"
#include <Wire.h>

//sonar config
int pingPin = 5;
int inPin = 4;
long duration, inches, cm;
int indec, cmdec;
int inchconv = 147;
int cmconv = 59;
String s1, s2;

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2 //GPIO2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String writeAPIKey = channelId;

float h;
float t;
float f;

//light sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

void setup()
{
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  dht.begin();
  connectWifi();
  //Serial.println("start...");
  //connectWifi();
  //updateDweet();
  //Serial.println("goint to sleep. good night.");
  //ESP.deepSleep(300*1000000, WAKE_RF_DEFAULT); // Sleep for 1 minutes
}

void loop()
{
  updateDweet();
  delay(500);
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

  //read sensor data
  readSensorData();
  
  //read sonar distance
  measureDistance();
  
  // We now create a URI for the request
  sensorValue = analogRead(analogInPin);
  String tsData = "field1=";
  tsData += s2;
  tsData += "&field2=";
  tsData += t;
  tsData += "&field3=";
  tsData += h;
  
  Serial.print("connected. distance=");
  Serial.println(s2);
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
    delay(250);
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

void measureDistance()
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
  Serial.println(duration);
  Serial.println(s1);
  Serial.println(s2);

  if (cm > 10)
  {
    Serial.println("---------------------------OPEN");
    ledsOff();
    digitalWrite(RED, HIGH);
  }
  else if (cm == 0)
  {
    Serial.println("---------------------------ERROR");
    ledsOff();
    blinkLed(BLUE);
    blinkLed(BLUE);
  }
  else
  {
    Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxCLOSED");
    ledsOff();
    digitalWrite(GREEN, HIGH);
  }
}

long microsecondsToInches(long microseconds)
{
  return microseconds / inchconv;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
}

void ledsOff() 
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

