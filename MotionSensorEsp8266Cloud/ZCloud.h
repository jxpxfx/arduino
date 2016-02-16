/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
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
    void retrieveDweet();
    void updateThingspeak(String tsData);
    void updateMakerChannel(bool isFirstTime, String event, float t);
    void ledsOff();
    void blinkLed(int color);
    void turnOff(int pin);
  private:
    int _pin;
};

#endif
