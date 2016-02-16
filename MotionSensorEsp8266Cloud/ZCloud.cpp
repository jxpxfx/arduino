#include "Arduino.h"
#include "ZCloud.h"
#include <ESP8266WiFi.h>
#include "configuration.h"

#define DEBUG true

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

const int maxConnectionRetries = 20;

String writeAPIKey = channelId;

ZCloud::ZCloud(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void ZCloud::connectWifi()
{
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  
  Serial.begin(9600);
  delay(10);
  for (int i = 0; i < networks; i++)
  {
    // We start by connecting to a WiFi network
    digitalWrite(BLUE, HIGH);
    Serial.print("Connecting to ");
    Serial.println(ssid[i]);
    
    WiFi.begin(ssid[i], password[i]);
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries <= maxConnectionRetries) {
      delay(500);
      Serial.print(".");
      tries++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      digitalWrite(BLUE, LOW);
      blinkLed(BLUE); 
      break;
    }
    else
    {
      Serial.print("Cannot connect to:");
      Serial.println(ssid[i]);
    }
  }
}

void ZCloud::updateDweet(String tsData)
{
  #if DEBUG
  Serial.print("connecting to ");
  Serial.println(dweetHost);
  #endif
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  while (!client.connect(dweetHost, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    //return;
    connectWifi();
  }
  
  // We now create a URI for the request
  String url = "/dweet/for/";
  url+= dweetThing;
  url+= "?";
  url+=tsData;

  #if DEBUG
  Serial.print("Requesting URL: ");
  Serial.println(url);
  #endif
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + dweetHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(50);
  
  // Read all the lines of the reply from server and print them to Serial
  #if DEBUG
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println("dweet updated");
  Serial.println("closing connection");
  #endif
}

void ZCloud::updateMakerChannel(bool isFirstTime, String event, float t)
{
  #if DEBUG
  Serial.print("connecting to ");
  Serial.println(hostMakerChannel);
  #endif 
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  while (!client.connect(hostMakerChannel, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    return;
    connectWifi();
  }
  
  // We now create a URI for the request
  String url;
  url += "/trigger/" + event + "/with/key/";
  
  url+= keyMakerChannel;
  url += "?value1=";
  if (isFirstTime)
  {
    url += "-999";
  }
  else
  {
    url += -1;
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
  
  #if DEBUG
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println("maker channel updated");
  Serial.println("closing connection");
  #endif
}

//format of tsData -> field1=value&field2=value&field3=value&field4=value
void ZCloud::updateThingspeak(String tsData)
{
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

void ZCloud::retrieveDweet()
{
}

//LEDS UTIL
void ZCloud::ledsOff() 
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

void ZCloud::blinkLed(int color) {
  //Serial.println("will blink now...");
  for (int i = 0; i < 2; i++) {
    digitalWrite(color, HIGH);
    delay(50);
    digitalWrite(color, LOW);
    delay(10);
  }
}

void ZCloud::turnOff(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

