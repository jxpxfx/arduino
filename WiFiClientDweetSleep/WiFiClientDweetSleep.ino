/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include <Ticker.h> // Ticker can periodically call a function
Ticker blinker; // Ticker object called blinker.
#include <ESP8266WiFi.h>

const char* ssid     = "WIFI-SSID";
const char* password = "WIFI-PASSWORD";

const char* host = "www.dweet.io";

//sensor
const int analogInPin = A0;
int sensorValue = 0;        // value read from the pot

//rgb 12, 13, 15
const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE
 
void setup() {
  Serial.println("start...");
  connectWifi();
  updateDweet();
  Serial.println("after updateDweet");
  blinker.attach(1.00, blink); // Ticker on blink function, call every 250ms
}

int value = 0;

void loop() {
  //updateDweet();
}

void blink()
{
  Serial.println("Good night. Will sleep now.");
  
  ESP.deepSleep(300000000, WAKE_RF_DEFAULT); // Sleep for 5 minutes
}
void connectWifi(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  
  Serial.begin(115200);
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
  int r = millis();
  String url = "/dweet/for/MY_THING?counter=";
  url += r;
  url += "&light=";
  url += sensorValue;
  //url+= value;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
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
  for (int i = 0; i < 5; i++) {
    digitalWrite(color, HIGH);
    delay(100);
    digitalWrite(color, LOW);
    delay(50);
  }
}

