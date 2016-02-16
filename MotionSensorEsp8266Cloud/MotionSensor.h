/*
*/
#ifndef MotionSensor_h
#define MotionSensor_h

#include "Arduino.h"

class MotionSensor
{
  public:
    MotionSensor(int pirPin);    
    void setupPir();
    bool checkPir();
  private:
    int _pirPin;
};

#endif
