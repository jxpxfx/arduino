/*
*/
#ifndef ZCloud_h
#define ZCloud_h

#include "Arduino.h"

class ZCloud
{
  public:
    ZCloud(int pin);
    void connectWifi();
    void updateDweet(String tsData);
    void updateThingspeak(String tsData);
    void updateMakerChannel(bool isFirstTime, String event, float t);
    void retrieveDweet();
    void ledsOff();
    void blinkLed(int color);
    void turnOn(int pin);
    void turnOff(int pin);
  private:
    void processResponse(String response);    
    int _pin;
};

#endif
