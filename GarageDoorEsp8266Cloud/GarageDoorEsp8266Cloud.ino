//configuration.h file
//const char* ssid     = "ssid";
//const char* password = "pass";
//
//const char* host = "api.thingspeak.com";
//const char* channelId = "channel";


#include "configuration.h"
#include "math.h"
#include <Wire.h>

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

bool prevIsOpen = -1;
bool isOpen = 0;

//long prevMillisMaker = -99999999;
//long prevMillisUpdateDweet = -99999999;

long prevMillisMaker = 0;
long prevMillisUpdateDweet = 0;

long updateMakerInterval = 30*60000;
long updateDweetInterval = 5000;

const int pinSwitch = 5; //Pin Reed GPIO4
int StatoSwitch = 0;

void setup()
{
  pinMode(pinSwitch, INPUT);
  dht.begin();
  connectWifi();
}

void loop()
{
  delay(100);
  ledsOff();
  //if ((unsigned long)(millis() - previousMillis) >= interval)
  long currMillis = millis();
  long elapsedMillisMaker = (unsigned long)(currMillis - prevMillisMaker); //currMillis - prevMillisMaker;
  long elapsedMillisDweet = (unsigned long)(currMillis - prevMillisUpdateDweet);//currMillis - prevMillisUpdateDweet;
  
  //Serial.println("elapsed:");
  //Serial.println(elapsedMillisDweet);
  //Serial.println("loop");
  
  //read switch
  StatoSwitch = digitalRead(pinSwitch);
  if (StatoSwitch == HIGH)
  {
    isOpen = false;
    digitalWrite(GREEN, HIGH);
  }
  else
  {
    isOpen = true;
    digitalWrite(RED, HIGH);
  }
  
  //tweet update logic every 5s
  if (elapsedMillisDweet > updateDweetInterval)
  {
    Serial.println("updateDweet 5s");
    //update dweet
    updateDweet();

    prevMillisUpdateDweet = currMillis;
  }  

  //maker channel update logic whenever status change
  if (isOpen != prevIsOpen)
  {
    Serial.println("status change");
    
    //update maker channel (status change notification) 
    updateMakerChannel();     

    //update dweet
    updateDweet();

    prevIsOpen = isOpen;    
  }

  //request DHT sensor data when cycle starts and after 30min
  //update thingspeak when a new reading is made
  if ((elapsedMillisMaker > updateMakerInterval)  || (prevMillisMaker == 0))
  {
    Serial.println("readerSensor");
    //read sensor data
    readSensorData();
    prevMillisMaker = currMillis;

    //update thingspeak
    updateThingspeak();
  }  
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

void updateThingspeak(){
  //Serial.print("updateThingspeak method");

  //Serial.print("connecting to ");
  //Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  while (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    //return;
    connectWifi();
  }
  
  // We now create a URI for the request
  sensorValue = analogRead(analogInPin);
  String tsData = "field1=";
  tsData += isOpen;
  tsData += "&field2=";
  tsData += t;
  tsData += "&field3=";
  tsData += h;
  
  Serial.print("connected TS. isOpen=");
  Serial.println(isOpen);
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(tsData.length());
  client.print("\n\n");

  client.print(tsData);
  delay(50);
  
  Serial.println("thingspeak updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
}

void updateMakerChannel(){
  Serial.print("connecting to ");
  Serial.println(hostMakerChannel);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(hostMakerChannel, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    return;
  }
  
  // We now create a URI for the request
  String url;
  if (isOpen)
  {
    url = "/trigger/garagedoor_open/with/key/";
  }
  else
  {
    url = "/trigger/garagedoor_closed/with/key/";
  }
  //String url = "/trigger/teste/with/key/";
  url+= keyMakerChannel;
  url += "?value1=";
  url += isOpen;
  url += "&value2=";
  url += t;
  url += "&value3=";
  url += millis();
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + hostMakerChannel + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(50);
  
  // Read all the lines of the reply from server and print them to Serial
  //while(client.available()){
  //  String line = client.readStringUntil('\r');
  //  Serial.print(line);
  //}
  
  Serial.println("maker channel updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
  
}

void updateDweet(){
  Serial.print("connecting to ");
  Serial.println(dweetHost);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(dweetHost, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    return;
  }
  
  // We now create a URI for the request
  String url = "/dweet/for/";
  url+= dweetThing;
  url += "?isOpen=";
  url += isOpen;
  url += "&temperature=";
  url += t;
  url += "&humidity=";
  url += h;
  url += "&millis=";
  url += millis();
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + dweetHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(50);
  
  // Read all the lines of the reply from server and print them to Serial
  //while(client.available()){
  //  String line = client.readStringUntil('\r');
  //  Serial.print(line);
  //}
  
  Serial.println("dweet updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
  
}

void blinkLed(int color) {
  Serial.println("will blink now...");
  for (int i = 0; i < 2; i++) {
    digitalWrite(color, HIGH);
    delay(50);
    digitalWrite(color, LOW);
    delay(10);
  }
}

void turnOff(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

void readSensorData() {
  int tries = 0;
  do {
    Serial.println("xDHTx");
    delay(250);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    f = dht.readTemperature(true); 
  } while (isnan(h) || isnan(t) || isnan(f) && tries++ < 8);
  
  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print("Humidity: ");
  //Serial.print(h);
  //Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  //Serial.print(f);
  //Serial.print(" *F\t");
  //Serial.print("Heat index: ");
  //Serial.print(hic);
  //Serial.print(" *C ");
  //Serial.print(hif);
  //Serial.println(" *F");
}

//void measureDistance()
//{
//  digitalWrite(pingPin, LOW);
//  delayMicroseconds(2);
//  digitalWrite(pingPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(pingPin, LOW);
//
//  duration = pulseIn(inPin, HIGH);
//
//  inches = microsecondsToInches(duration);
//  indec = (duration - inches * inchconv) * 10 / inchconv;
//  cm = microsecondsToCentimeters(duration);
//  cmdec = (duration - cm * cmconv) * 10 / cmconv;
//  s1 = String(inches) + "." + String(indec) + "in" + "     ";
//  s2 = String(cm) + "." + String(cmdec) + "cm" + "     ";
//  //Serial.println(duration);
//  //Serial.println(s1);
//  Serial.println(s2);
//
//  if (cm > threshold)
//  {
//    isOpen = 1;
//    //Serial.println("---------------------------OPEN");
//    ledsOff();
//    digitalWrite(RED, HIGH);
//  }
//  else if (cm == 0)
//  {
//    isOpen = 1;
//    //Serial.println("---------------------------ERROR");
//    ledsOff();
//    blinkLed(BLUE);
//    blinkLed(BLUE);
//  }
//  else
//  {
//    isOpen = 0;
//    //Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxCLOSED");
//    ledsOff();
//    digitalWrite(GREEN, HIGH);
//  }
//}

//long microsecondsToInches(long microseconds)
//{
//  return microseconds / inchconv;
//}
//
//long microsecondsToCentimeters(long microseconds)
//{
//  return microseconds / cmconv;
//}

void ledsOff() 
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

