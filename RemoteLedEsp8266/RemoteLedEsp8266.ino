#include <ArduinoJson.h>
#include "configuration.h"
//#include "math.h"
//#include <Wire.h>

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

int pushButton = 5;

#include <ESP8266WiFi.h>

String writeAPIKey = channelId;

//light sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

bool is1stTime = true;

long prevMillisMaker = 0;
long prevMillisUpdateDweet = 0;

long updateMakerInterval = 30*60000;
long updateDweetInterval = 5000;

bool isAlertsOn = true;

void setup()
{
  pinMode(pushButton, INPUT);
  
  connectWifi();
  ledsOff();
}

void loop()
{
  buttonAlerts();
  
  delay(100);
  //ledsOff();
  //if ((unsigned long)(millis() - previousMillis) >= interval)
  long currMillis = millis();
  long elapsedMillisMaker = (unsigned long)(currMillis - prevMillisMaker); //currMillis - prevMillisMaker;
  long elapsedMillisDweet = (unsigned long)(currMillis - prevMillisUpdateDweet);//currMillis - prevMillisUpdateDweet;
  
  //tweet update logic every 5s
  if (elapsedMillisDweet > updateDweetInterval)
  {
    //Serial.println("updateDweet 5s");    
    getDweet();

    prevMillisUpdateDweet = currMillis;
  }  
}

void buttonAlerts()
{
    //read button state (toggle)
    // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  Serial.println("button state=");
  Serial.println(buttonState);
  if (buttonState == 1)
  {
    isAlertsOn = !isAlertsOn;
    Serial.println("flipped isAlertsOn=");
    Serial.println(isAlertsOn);
  }

  Serial.println("isAlertsOn=");
  Serial.println(isAlertsOn);
}

void connectWifi(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  ledsOff();
  
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network
  //digitalWrite(BLUE, HIGH);
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
  //digitalWrite(BLUE, LOW);
  blinkLed(BLUE);
}


void getDweet(){
  //Serial.print("connecting to ");
  //Serial.println(dweetHost);

  //blinkLed(GREEN);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(dweetHost, httpPort)) {
    Serial.println("connection failed");
    blinkLed(RED);
    return;
  }
  
  // We now create a URI for the request
  String url = "/get/latest/dweet/for/";
  url+= dweetThing;
  
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + dweetHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(50);

  //blinkLed(BLUE);
  // Read all the lines of the reply from server and print them to Serial
  String response;
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line);
    response += line;
  }  
   
  processResponse(response);
  //blinkLed(GREEN);
  //Serial.println("closing connection");
  //processResponse(client);
  delay(5000);
  
}

void processResponse(String response) {
   
  int openBracket = response.indexOf('{');
  int closeBracket = response.lastIndexOf('}');
  String responseJson = response.substring(openBracket, closeBracket+1);

//  Serial.println("responseJson:");
  Serial.println(responseJson);
//  Serial.println("got dweet");  
  
  DynamicJsonBuffer  jsonBuffer;

    // Root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& root = jsonBuffer.parseObject(responseJson);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  // Fetch values.
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root["time"].as<long>();
  const char* sensor = root["this"];
  String thing = root["with"][0]["thing"].asString();
  String temperature = root["with"][0]["content"]["temperature"].asString();
  
  
  Serial.println("Values:");
  Serial.println(thing);
  Serial.println(temperature);
  
  String isOpen = root["with"][0]["content"]["isOpen"].asString();
  bool isOpenB = isOpen.toInt();
  Serial.println(isOpenB);
  
  String mil = root["with"][0]["content"]["millis"].asString();
  long milLong = mil.toInt();
  Serial.println(milLong);

  if (isAlertsOn)
  {
    if (milLong % 2 == 0) {
      turnOn(GREEN);
    }
    else
    {
      turnOn(RED);
    }
  }
  else
  {
    turnOn(BLUE);
  }
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

void turnOn(int pin) {
  ledsOff();
  digitalWrite(pin, HIGH);
}

void turnOff(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

void ledsOff() 
{
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

