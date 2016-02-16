//configuration.h file
//const char* ssid     = "ssid";
//const char* password = "pass";
//
//const char* host = "api.thingspeak.com";
//const char* channelId = "channel";


#include "configuration.h"
#include "math.h"
#include <Wire.h>

//PIR
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 1000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 4;    //the digital pin connected to the PIR sensor's output

boolean isMotionDetected = false;
boolean isMotionDetectedPrevious = -1;
//END PIR

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

#include <ESP8266WiFi.h>
#include "DHT.h"

//#define DHTPIN 2 //GPIO2
#define DHTPIN 14 //GPIO14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String writeAPIKey = channelId;

float h;
float t;
float f;

//light sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

//bool prevIsOpen = -1;
//bool isOpen = 0;

bool is1stTime = true;

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
  //dht.begin();
  connectWifi();
  //readSensorData();
  setupPir();
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
  
  //read pir
  checkPir();  
  
  //tweet update logic every 5s
  if (elapsedMillisDweet > updateDweetInterval)
  {
    //Serial.println("updateDweet 5s");
    //update dweet
    updateDweet();

    prevMillisUpdateDweet = currMillis;
  }  

  //maker channel update logic whenever status change
  if (isMotionDetected != isMotionDetectedPrevious)
  {
    //Serial.println("status change");
    
    //update maker channel (status change notification) 
    if (is1stTime)
    {
      is1stTime = false;
    }
    else
    {
      updateMakerChannel(false);     
    }

    //update dweet
    updateDweet();

    isMotionDetectedPrevious = isMotionDetected;    
  }

  //request DHT sensor data when cycle starts and after 30min
  //update thingspeak when a new reading is made
  if ((elapsedMillisMaker > updateMakerInterval)  || (prevMillisMaker == 0))
  {
    //Serial.println("readerSensor");
    //read sensor data
    //readSensorData();
    prevMillisMaker = currMillis;

    //update thingspeak
    updateThingspeak();
  }  
}

void checkPir()
{
if(digitalRead(pirPin) == HIGH){
   digitalWrite(RED, HIGH);   //the led visualizes the sensors output pin state
   if(lockLow){  
     //makes sure we wait for a transition to LOW before any further output is made:
     lockLow = false;            
     Serial.println("---");
     Serial.print("motion detected at ");
     Serial.print(millis()/1000);
     Serial.println(" sec"); 
     delay(50);
     }         
     takeLowTime = true;
     isMotionDetected = true;
   }

 if(digitalRead(pirPin) == LOW){       
   digitalWrite(RED, LOW);  //the led visualizes the sensors output pin state

   if(takeLowTime){
    lowIn = millis();          //save the time of the transition from high to LOW
    takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
   //if the sensor is low for more than the given pause, 
   //we assume that no more motion is going to happen
   if(!lockLow && millis() - lowIn > pause){  
       //makes sure this block of code is only executed again after 
       //a new motion sequence has been detected
       lockLow = true;                        
       Serial.print("motion ended at ");      //output
       Serial.print((millis() - pause)/1000);
       Serial.println(" sec");
       delay(50);
       isMotionDetected = false;
     }
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
  digitalWrite(BLUE, HIGH);
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
  digitalWrite(BLUE, LOW);
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
  tsData += isMotionDetected;
  tsData += "&field2=";
  tsData += t;
  tsData += "&field3=";
  tsData += h;
  tsData += "&field4=";
  tsData += millis();
  
  //Serial.print("connected TS. isOpen=");
  //Serial.println(isOpen);
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
  
  //Serial.println("thingspeak updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
}

void updateMakerChannel(bool isFirstTime){
  //Serial.print("connecting to ");
  //Serial.println(hostMakerChannel);
  
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
  if (isMotionDetected)
  {
    url = "/trigger/motion_started/with/key/";
  }
  else
  {
    url = "/trigger/motion_ended/with/key/";
  }
  //String url = "/trigger/teste/with/key/";
  url+= keyMakerChannel;
  url += "?value1=";
  if (isFirstTime)
  {
    url += "-999";
  }
  else
  {
    url += isMotionDetected;
  }  
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
  
  //Serial.println("maker channel updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
  
}

void updateDweet(){
  //Serial.print("connecting to ");
  //Serial.println(dweetHost);
  
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
  url += "?isMotionDetected=";
  url += isMotionDetected;
  url += "&temperature=";
  url += t;
  url += "&humidity=";
  url += h;
  url += "&millis=";
  url += millis();
  
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  
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
  
  //Serial.println("dweet updated");
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //delay(2000);
  
}

void blinkLed(int color) {
  //Serial.println("will blink now...");
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
    //Serial.println("xDHTx");
    delay(250);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    f = dht.readTemperature(true); 
  } while (isnan(h) || isnan(t) || isnan(f) && tries++ < 8);
}

void setupPir()
{
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);  
}

void ledsOff() 
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

