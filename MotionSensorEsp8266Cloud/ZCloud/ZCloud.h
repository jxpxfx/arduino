/*
*/
#ifndef ZCloud_h
#define ZCloud_h

#include "Arduino.h"

class ZCloud
{
  public:
    ZCloud(char* ssid[3], char* password[3]);
    void connectWifi();
    void updateDweet(String tsData);
    void updateThingspeak(String tsData);
    void updateMakerChannel(bool isFirstTime, String event, float t);
    void retrieveDweet();
    void ledsOff();
    void blinkLed(int color);
    void turnOn(int pin);
    void turnOff(int pin);
    void setThingspeakChannelId(char* thingspeakChannelId);
    void setKeyMakerChannel(char* keyMakerChannel);
    void setDweetThing (char* dweetThing);
    void setDweetThingRead(char* dweetThingRead);
  private:
    char* _ssid[3];
    char* _password[3];    
    void processResponse(String response);    
    int _pin;
    
    char* _thingspeakChannelId;
    char* _keyMakerChannel;
    char* _dweetThing;
    char* _dweetThingRead;
};

#endif
