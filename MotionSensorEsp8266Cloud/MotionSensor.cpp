#include "Arduino.h"
#include "MotionSensor.h"

#define DEBUG true

const int RED = 15;  //RED
const int GREEN = 12; //GREEN
const int BLUE = 13; //BLUE

//PIR
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 1000;  

boolean lockLow = true;
boolean takeLowTime;  

//int pirPin = 4;    //the digital pin connected to the PIR sensor's output

boolean motionDetected = false;
//END PIR

MotionSensor::MotionSensor(int pirPin)
{
  _pirPin = pirPin;
}

void MotionSensor::setupPir()
{
  pinMode(_pirPin, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print("_ ");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);  
}

bool MotionSensor::checkPir()
{
if(digitalRead(_pirPin) == HIGH){
   digitalWrite(RED, HIGH);   //the led visualizes the sensors output pin state
   digitalWrite(GREEN, LOW);   //the led visualizes the sensors output pin state
   if(lockLow){  
     //makes sure we wait for a transition to LOW before any further output is made:
     lockLow = false;            
     #if DEBUG
     Serial.println("---");
     Serial.print("motion detected at ");
     Serial.print(millis()/1000);
     Serial.println(" sec"); 
     #endif
     delay(50);
     }         
     takeLowTime = true;
     motionDetected = true;
   }

 if(digitalRead(_pirPin) == LOW){       
   digitalWrite(RED, LOW);  //the led visualizes the sensors output pin state
   digitalWrite(GREEN, HIGH);  //the led visualizes the sensors output pin state

   if(takeLowTime){
    lowIn = millis();          //save the time of the transition from high to LOW
    takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
   //if the sensor is low for more than the given pause, 
   //we assume that no more motion is going to happen
   if(!lockLow && millis() - lowIn > pause){  
       //makes sure this block of code is only executed again after 
       //a new motion sequence has been detected
       lockLow = true;      
       #if DEBUG                  
       Serial.print("motion ended at ");      //output
       Serial.print((millis() - pause)/1000);
       Serial.println(" sec");
       #endif
       delay(50);
       motionDetected = false;
     }
   }
   return motionDetected;
}


