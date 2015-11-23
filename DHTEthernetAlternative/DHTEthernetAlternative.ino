/*

 Arduino --> ThingSpeak Channel via Ethernet
 
 The ThingSpeak Client sketch is designed for the Arduino and Ethernet.
 This sketch updates a channel feed with an analog input reading via the
 ThingSpeak API (https://thingspeak.com/docs)
 using HTTP POST. The Arduino uses DHCP and DNS for a simpler network setup.
 The sketch also includes a Watchdog / Reset function to make sure the
 Arduino stays connected and/or regains connectivity after a network outage.
 Use the Serial Monitor on the Arduino IDE to see verbose network feedback
 and ThingSpeak connectivity status.
 
 Getting Started with ThingSpeak:
 
   * Sign Up for New User Account - https://thingspeak.com/users/new
   * Create a new Channel by selecting Channels and then Create New Channel
   * Enter the Write API Key in this sketch under "ThingSpeak Settings"
 
 Arduino Requirements:
 
   * Arduino with Ethernet Shield or Arduino Ethernet
   * Arduino 1.0+ IDE
   
  Network Requirements:

   * Ethernet port on Router    
   * DHCP enabled on Router
   * Unique MAC Address for Arduino
 
 Created: October 17, 2011 by Hans Scharler (http://www.nothans.com)
 
 Additional Credits:
 Example sketches from Arduino team, Ethernet by Adrian McEwen
 
*/

#include <SPI.h>
#include <Ethernet.h>

#include "DHT.h"

#define DHTPIN 4     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

float h;
float t;

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "NOK2VQYZ99P30MTY";
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

void setup()
{
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  
  // Start Ethernet on Arduino
  startEthernet();
}

void loop()
{
  // Read value from Analog Input Pin 0
  String analogValue0 = String(analogRead(A0), DEC);
  
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    //Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
  
  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    readSensorData();
    String d = "field1=";
    d +=t;
    d +="&field2=";
    d +=h;
    updateThingSpeak(d);
  } else {
    //Serial.print(".");
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {startEthernet();}
  
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startEthernet()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
  }
  
  delay(1000);
}

void readSensorData() {
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

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
