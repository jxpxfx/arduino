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

#include "MotionSensor.h"
MotionSensor motionSensor(4);
boolean isMotionDetected = false;
boolean isMotionDetectedPrevious = -1;

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

bool is1stTime = true;

long prevMillisMaker = 0;
long prevMillisUpdateDweet = 0;

long updateMakerInterval = 30*60000;
long updateDweetInterval = 5000;

void setup()
{
  zcloud.connectWifi();
  motionSensor.setupPir();    
  zcloud.ledsOff();
  //dht.begin();
  //readSensorData();
}

void loop()
{
  //read pir
  isMotionDetected = motionSensor.checkPir();
  delay(100);
  //zcloud.ledsOff();
  //if ((unsigned long)(millis() - previousMillis) >= interval)
  long currMillis = millis();
  long elapsedMillisMaker = (unsigned long)(currMillis - prevMillisMaker); //currMillis - prevMillisMaker;
  long elapsedMillisDweet = (unsigned long)(currMillis - prevMillisUpdateDweet);//currMillis - prevMillisUpdateDweet;
  
  
  //tweet update logic every 5s
  if (elapsedMillisDweet > updateDweetInterval)
  {
    updateDweet();

    prevMillisUpdateDweet = currMillis;
  }  

  //maker channel update logic whenever status change
  if (isMotionDetected != isMotionDetectedPrevious)
  {
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
