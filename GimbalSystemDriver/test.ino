
#include "GimbalSystemDriver.h"

#define REFRESH_PERIOD 0.00125 //2.5ms - 400Hz, 1.25ms - 800Hz
#define DEADZONE 5

//Instantiate objects
elapsedMillis sinceRead;
GimbalSystemDriver throttle, steering;
FilterIIR velocity_filter_throttle, velocity_filter_steering, an_filter1, an_filter2;

float spring = 0;
int pos = 0;
float descur = 0;
float sped = 0;
float lastpos = 0;
float damper = 0;
float time_interval = 0;
void setup() {
  Serial.begin(9600);

  throttle.configureINA219(0);
  throttle.setMotorPins(7,8,9);
  throttle.configurePWM(8);
  throttle.setEncoderPins(3,4);
  throttle.setCCPID(0.1, 1.5, 0.0, 1.0);
  throttle.configureFilter1(1,0.95);
  throttle.configureFilter2(1,0.95);
  throttle.findZero();

  /*
  steering.configureINA219(0);
  steering.setMotorPins(7,8,9);
  steering.configurePWM(8);
  steering.setEncoderPins(3,4);
  steering.setCCPID(0.1, 1.5, 0.0, 1.0);
  steering.configureFilter1(1,0.95);
  steering.configureFilter2(1,0.95);
  steering.findZero();
  */


  
  velocity_filter_throttle.setStages(1);
  velocity_filter_throttle.setConstant(0.925);  
  an_filter1.setStages(1);
  an_filter1.setConstant(0.9);
  an_filter2.setStages(1);
  an_filter2.setConstant(0.9);
}

void loop() {
  if (sinceRead >= (REFRESH_PERIOD*1000)){
    time_interval = sinceRead/1000.0; //If our uC waits longer than expected, take into account
    sinceRead=0; //Reset "timer"
    pos = throttle.readPos();
    sped = (pos - lastpos)/time_interval;
    lastpos = pos;
    sped = velocity_filter_throttle.filter(sped);

    if (pos == lastpos)
      throttle.setCCPID(0.1, 1.5, 0.0, 1.0); //This zero's integrator term. We can also exploit this to do gain scheduling in different regions
    else if ((pos>0) && (lastpos<0))
      throttle.setCCPID(0.1, 1.5, 0.0, 1.0); 
    else if ((pos<0) && (lastpos>0))
      throttle.setCCPID(0.1, 1.5, 0.0, 1.0);
    
    
    if (pos>DEADZONE)
      descur = (-spring*pos - damper*sped);
    else if (pos<(-DEADZONE))
      descur = 1.0*(-spring*pos - damper*sped);
    else
      descur = 0;
      
    throttle.driveCurrent(descur, time_interval);
    Serial.print(pos);
    Serial.print("  ");
    Serial.print(descur);
    Serial.print("  ");
    Serial.print(-spring*pos);
    Serial.print("  ");
    Serial.print(damper*sped);
    Serial.print("  ");
    Serial.print(throttle.outp);
    Serial.print("  ");
        //Serial.print(spring);
    //Serial.print("  ");
        //Serial.print(damper);
    //Serial.print("  ");
    //Serial.print(throttle.gas);
    //Serial.print("  ");
    //Serial.print(throttle.readCurrent_raw());
    //Serial.print("  ");
    //Serial.print(throttle.readCurrent_filter1());
    //Serial.print("  ");
    Serial.println(throttle.readCurrent_filter2());
  }
  else{
    spring = 10.0*(an_filter1.filter(analogRead(14))/1024.0);
    damper = 10.0*(an_filter2.filter(analogRead(15))/1024.0);
  }
}
