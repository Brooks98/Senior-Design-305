//Team 305, Haptic Feedback Controller
#include "Adafruit_INA219.h"
#include "FilterIIR.h"
#include <Wire.h>
#include <Encoder.h>

#ifndef GIMBAL_SYS_DRIVE
  #define GIMBAL_SYS_DRIVE
#endif

//Default Pin Assignments
#define DEFAULT_MENA 7
#define DEFAULT_MIN1 8
#define DEFAULT_MIN2 9
#define DEFAULT_ENC1 3
#define DEFAULT_ENC2 4

//Default System parameters
#define DEFAULT_MOTOR_THRESH 75 //In Percent
#define DEFAULT_STALL_THRESH 5 //In Percent
#define DEFAULT_INTEGRAL_LIMIT 200
#define DEFAULT_REFRESH_PERIOD 0.0025 //100Hz
#define DEFUALT_PWM_RES 8

//Current Sensor Parameters
#define DEFAULT_INA219_ADR 1

#define DEFAULT_IIR_A 0.8

enum motormode {
  FORWARD=0,
  REVERSE=1,
  FASTSTOP=2,
  FREESTOP=3,
};

class GimbalSystemDriver{
  public:
    //utility
    GimbalSystemDriver(void); //Done
    
    int setMotorPins(int _ena, int _in1, int _in2); //Done
    int setEncoderPins(int _enc1, int _enc2); //Done
    int configureINA219(int _adr); //Done
    int configurePWM(int _resPWM); //Done
    
    int configureFilter1(int _stages, float _a); //Done
    int configureFilter2(int _stages, float _a); //Done
    
    //Parameterization
    int setMotorKs(float _kv); //IP
    //int setGimbalParams();
    int setCCPID(float _kp, float _ki, float _kd, float _kc); //Set current/torque control PID parameters //Done
    //int setSCPID(float kp, float ki, float kd, float kc); //Set speed control PID parameters
    int setSamplePeriod(float _T); //Done
    
    //Action!
    //int driveSpeed(const int speed);
    int driveCurrent(float cur, float t); //Drives desired current, takes into account time t since last command
    //int driveTorque(const int torque);
    int driveRaw(int mode, int gas);
    int findZero(void);
   
    //Read Data
    //float readSpeed(void);
    int readPos(void); //Done
    float readCurrent_raw(void); //Done
    float readCurrent_filter1(void); //Done
    float readCurrent_filter2(void); //Done
    //float readTorque(void);

    float outp;
    int gas;
    
  private:
    //Pins
    int ena, in1, in2; //Motor Pins
    int enc1, enc2; //Encoder Pins
    //System Parameters
    float refresh_period; //In seconds
    int resPWM; //PWM resolution in bits
    float motor_thresh;
    
    //Motor Parameters
    float stall_thresh;
    float kv;
    float km;
    
    //INA_219 Output Data
    Adafruit_INA219 ina219;
    float shuntvoltage;
    float busvoltage;
    float current_mA;
    float loadvoltage;
    float power_mW;
    float current_mA_fil1, current_mA_fil2;
    
    //Encoder
    Encoder *axis;
    int enc_offset;
    
    //Filter Data IIR Filter
    FilterIIR filter1, filter2;
    float a1, a2, a3, a4;
    int f1_stages, f2_stages;
  
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
    //PID Utility
    float integral_limit;
};
