//configuration.h file
//const char* ssid     = "ssid";
//const char* password = "pass";
//
//const char* host = "api.thingspeak.com";
//const char* channelId = "channel";

//#include "configuration.h"
#include "math.h"
#include <Wire.h>

#include "ZCloud.h"
ZCloud zcloud(13);

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

//String writeAPIKey = channelId;

float h;
float t;
float f;

//light sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

bool is1stTime = true;


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
  zcloud.connectWifi();
  //readSensorData();
  setupPir();
}

void loop()
{
  delay(100);
  zcloud.ledsOff();
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
      zcloud.updateMakerChannel(false, isMotionDetected ? "motion_started" : "motion_ended", t);     
    }

    updateDweet();

    updateThingspeak();

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

    //updateThingspeak();
  }  
}

void updateDweet()
{
      //update dweet
    String tsData = "isMotionDetected=";
    tsData += isMotionDetected;
    tsData += "&temperature=";
    tsData += t;
    tsData += "&humidity=";
    tsData += h;
    tsData += "&millis=";
    tsData += millis();
    zcloud.updateDweet(tsData);
}

void updateThingspeak()
{
      //update thingspeak
    String tsData = "field1=";
    tsData += isMotionDetected;
    tsData += "&field2=";
    tsData += t;
    tsData += "&field3=";
    tsData += h;
    tsData += "&field4=";
    tsData += millis();
    zcloud.updateThingspeak(tsData);
}

void updateMakerChannel()
{
  
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
    Serial.print("_ ");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);  
}
