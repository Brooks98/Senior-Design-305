//Team 305, Haptic Feedback Controller
#include "Adafruit_INA219.h"

#ifndef GIMBAL_SYS_DRIVE
#define GIMBAL_SYS_DRIVE


class GimbalSystemDriver{
  public:
    
  
    //utility
    GimbalSystemDriver(void);
    int setMotorPins(const int ena, const int in1, const int in2);
    //int setINAPins(); Uses Default Teensy I2C Pins. 
    int setINAaddr(int adr); //Need this as we will have 2 sensors on one I2C Buse
    int setEncoderPins();
    const int configurePins(void);
    const int configureINA(void);
    
    //Parameterization
    int setMotorParams();
    int setGimbalParams();
    int setCCPID(float kp, float ki, float kd, float kc);
    int setSCPID(float kp, float ki, float kd, float kc);
    int setSamplePeriod(float T);
    
    //Action!
    int driveSpeed(const int speed);
    int driveCurrent(const int cur);
    int driveTorque(const int torque);
    int findZero(void);
  
    //Read Data
    float readSpeed(void);
    float readCurrent_raw(void);
    float readCurrent_filtered(void);
    float readTorque(void);
  
};
