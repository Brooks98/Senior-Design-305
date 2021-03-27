//Team 305, Haptic Feedback Controller
#include "Adafruit_INA219.h"

#ifndef GIMBAL_SYS_DRIVE
#define GIMBAL_SYS_DRIVE

#define MAX_SAMPLES 100 //For MAF, IIR, or FIR filter buffer
#define DEFAULT_MENA 9
#define DEFAULT_MIN1 10
#define DEFAULT_MIN2 11
#define DEFAULT_ENC1 13
#define DEFAULT_ENC2 14
#define DEFAULT_NUMSAMPLES 8
#define DEFAULT_MOTOR_THRESH 10 //In Percent
#define DEFAULT_STALL_THRESH 25 //In Percent
#define DEFAULT_REFRESH_PERIOD 0.01 //100Hz
class GimbalSystemDriver{
  public:
    //utility
    GimbalSystemDriver(void);
    int setMotorPins(int ena, int in1, int in2);
    //int setINAPins(); Uses Default Teensy I2C Pins. 
    int setINAaddr(int adr); //Need this as we will have 2 sensors on one I2C Bus. Wire extra pins to GND or VCC to set the adr on the INA devices
    int setEncoderPins(int in1, int in2);
    int configureINA(void);
    
    //Parameterization
    int setMotorKs(float kv);
    //int setGimbalParams();
    int setCCPID(float kp, float ki, float kd, float kc); //Set current/torque control PID parameters
    int setSCPID(float kp, float ki, float kd, float kc); //Set speed control PID parameters
    int setSamplePeriod(float T);
    
    //Action!
    int driveSpeed(const int speed);
    int driveCurrent(const int cur);
    //int driveTorque(const int torque);
    int findZero(void);
   
    //Read Data
    float readSpeed(void);
    float readCurrent_raw(void);
    float readCurrent_filtered(void);
    float readTorque(void);
    
    //Pins
    int ena, in1, in2; //Motor Pins
    int enc1, enc2; //Encoder Pins
    //System Parameters
    float refresh_period; //In seconds
    float motor_thresh;
    //Motor Parameters
    float stall_thresh;
    float kv;
    float km;
    
    //INA_219 Output Data
    float shuntvoltage;
    float busvoltage;
    float current_mA;
    float loadvoltage;
    float power_mW;

    //Filter Data (Should use a list type structure for performance eventually)
    float num_samples;
    float currents[MAX_SAMPLES]; float speeds[MAX_SAMPLES];
  
    //PID Controller Setpoints
    float desspeed; float descur; float destorque;
    //PID Controller Constants
    float kp_speed; float kp_cur; //Proportional constant
    float kd_speed; float kd_cur; //Derivative constant
    float ki_speed; float ki_cur; //Integral constant
    float kc_speed; float kc_cur; //Overall Controller Gain
    //PID Error Terms
    float nowerr_c; float nowerr_s;
    float lasterr_c; float lasterr_s;
    float deriv_c; float deriv_s;
    float integ_c; float integ_s;
};
