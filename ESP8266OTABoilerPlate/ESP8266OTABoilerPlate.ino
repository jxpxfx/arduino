/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
  Thanks to https://github.com/tzapu/WiFiManager
  Thanks to https://github.com/JoaoLopesF/RemoteDebug
*/

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

// Remote debug over telnet - not recommended for production, only for development
#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug

// Wifi Manager
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

//web update OTA
const char* host = "esp8266-webupdate";
//const char* ssid = "";
//const char* password = "";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

//Remote Debug
RemoteDebug Debug;
int count = 0;

void setup(void){

  setupNetwork();

  //YOUR SETUP CODE GOES HERE
  
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(15, OUTPUT);
  
  //YOUR SETUP CODE ENDS HERE
}

void loop(void){
  //Web Server
  httpServer.handleClient();

  //YOUR LOOP CODE GOES HERE
  
    Debug.println(count++);
  }
  
  
  //Blink LED
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
  delay(500);    
  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
  delay(500);    
  //YOUR LOOP CODE ENDS HERE
  
  // Remote debug over telnet
  Debug.handle();
}

void setupNetwork() {
  //Wifi Manager
  WiFiManager wifiManager;
  //first parameter is name of access point, second is the password
//  wifiManager.autoConnect();

  //we don't need wifi here as the wifi manager is taking care of the connection
    
//  Serial.begin(115200);
//  Serial.println();
//  Serial.println("Booting Sketch...");
//  WiFi.mode(WIFI_AP_STA);
//  WiFi.begin(ssid, password);
//
//  while(WiFi.waitForConnectResult() != WL_CONNECTED){
//    WiFi.begin(ssid, password);
//    Serial.println("WiFi failed, retrying.");
//  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);

  // Initialize the telnet server of RemoteDebug  
  Debug.begin("Telnet_HostName"); // Initiaze the telnet server
  // OR
  Debug.begin(host); // Initiaze the telnet server - HOST_NAME is the used in MDNS.begin
  
  Debug.setResetCmdEnabled(true); // Enable the reset command
  //Debug.showTime(true); // To show time
  // Debug.showProfiler(true); // To show profiler - time between messages of Debug  
}

void printMACAddress() {
  if (Debug.ative(Debug.DEBUG)) {    
    byte mac[6];
    WiFi.macAddress(mac);
    Debug.print("MAC: ");
    Debug.print(mac[0],HEX);
    Debug.print(":");
    Debug.print(mac[1],HEX);
    Debug.print(":");
    Debug.print(mac[2],HEX);
    Debug.print(":");
    Debug.print(mac[3],HEX);
    Debug.print(":");
    Debug.print(mac[4],HEX);
    Debug.print(":");
    Debug.println(mac[5],HEX);
  }  
}

