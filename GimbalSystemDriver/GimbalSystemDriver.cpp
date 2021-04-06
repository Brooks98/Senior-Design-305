//Here's the implementation
#include "GimbalSystemDriver.h"

/*
 *  Default constructor, sets all things to default values
 */
GimbalSystemDriver::GimbalSystemDriver(void){
  km = 1; kv = 1;
  ena = DEFAULT_MENA; in1 = DEFAULT_MIN1; in2 = DEFAULT_MIN2; //Use Pins 9,10,11 as default for motor
  enc1 = DEFAULT_ENC1; enc2 = DEFAULT_ENC2; //Set default Encoder Pins
  motor_thresh = DEFAULT_MOTOR_THRESH; 
  stall_thresh = DEFAULT_STALL_THRESH;
  integral_limit = DEFAULT_INTEGRAL_LIMIT;
  refresh_period = DEFAULT_REFRESH_PERIOD; //100Hz operating frequency
  
  desspeed = descur = destorque = 0; //Zero setpoint
  nowerr_c = lasterr_c = deriv_c = integ_c = 0; //Zero PID error terms
  nowerr_s = lasterr_s = deriv_s = integ_s = 0;
  enc_offset = 0;
  setEncoderPins(enc1,enc2);
  ina219.setCalibration_16V_400mA();
  //Configure Pins
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);
}

/*
 *  setMotorPins: configures pins for enable, input1, and input2. for L298N Inputs correspond to clockwise and counterclockwise.
 *  INPUTS _ena, _in1, _in2 - L298 Motor driver pins, _in1, and _in2 MUST be FlexPWM pins
 *  NOTES - MUST USE A FLEXPWM timer controlled pin! NOT a QuadTimer!
 */
int GimbalSystemDriver::setMotorPins(int _ena = DEFAULT_MENA, int _in1 = DEFAULT_MIN1, int _in2 = DEFAULT_MIN2){
  ena = _ena;
  in1 = _in1;
  in2 = _in2;
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  return 0;
}

/*
 *  setEncoderPins: Sets pins to be used by HardwareEncoder Library
 *  INPUTS _enc1, _enc2: Pin assignments for encoder
 *  NOTES: Library usage link here
 */
int GimbalSystemDriver::setEncoderPins(int _enc1 = DEFAULT_ENC1, int _enc2 = DEFAULT_ENC2){
  enc1 = _enc1;
  enc2 = _enc2;
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);
  if (axis!=NULL)
    delete axis;
  axis = new Encoder(enc1,enc2);
  return 0;
}

/*
 * setMotorKs: Set motor kv, then km is calculated
 *  INPUTS _kv: motor voltage/speed constant
 *  NOTES: Still need to calculate km here
 */
int GimbalSystemDriver::setMotorKs(float _kv){
  kv = _kv;
  km = 1 * kv;
  return 0;
}

/*
 * configurePWM: configures PWM resolution and corresponding optimal frequencies
 *  INPUTS res - PWM resolution, from 8-10, inclusive
 *  NOTES - Ideal PWM frequencies specified at https://www.pjrc.com/teensy/td_pulse.html
 *        - MUST USE A FLEXPWM timer controlled pin! NOT a QuadTimer!
 */
int GimbalSystemDriver::configurePWM(int _resPWM = DEFUALT_PWM_RES){
  if(_resPWM == 8){
    analogWriteFrequency(in1, 22500);
    analogWriteFrequency(in2, 22500);  
    analogWriteResolution(8);
  }
  else if(_resPWM == 9){
    analogWriteFrequency(in1, 292968.75);
    analogWriteFrequency(in2, 292968.75);  
    analogWriteResolution(9);
  } 
  else if(_resPWM == 10){
    analogWriteFrequency(in1, 146484.38);
    analogWriteFrequency(in2, 146484.38);  
    analogWriteResolution(10);
  }
  else{
    return -1;
  }
  resPWM = _resPWM;
  return 0; 
}

int GimbalSystemDriver::configureINA219(int _adr = DEFAULT_INA219_ADR){
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
    return -1; //Failure
  }
  return 0;
}

int GimbalSystemDriver::setSamplePeriod(float _T){
  if((_T>0.002) & (_T<0.1))
    refresh_period = _T; return 0;
  return -1;
}

int GimbalSystemDriver::setCCPID(float _kp, float _ki, float _kd, float _kc){
  kp_cur = _kp; ki_cur = _ki; kd_cur = _kd; kc_cur = _kc;
  integ_c = 0; //Zero Integrator Term so it's not left wound up
  return 0;
}

float GimbalSystemDriver::readCurrent_raw(void){
  current_mA = ina219.getCurrent_mA();
  current_mA_fil1 = filter1.filter(current_mA);
  current_mA_fil2 = filter2.filter(current_mA_fil1);
  return current_mA;
  }

float GimbalSystemDriver::readCurrent_filter1(void){
  //readCurrent_raw();
  return current_mA_fil1; }

float GimbalSystemDriver::readCurrent_filter2(void){
  //readCurrent_raw();
  return current_mA_fil2; }

int GimbalSystemDriver::configureFilter1(int _stages, float _a){
  filter1.setStages(_stages);
  filter1.setConstant(_a);
  return 0;
}
int GimbalSystemDriver::configureFilter2(int _stages, float _a){
  filter2.setStages(_stages);
  filter2.setConstant(_a);
  return 0;
}

int GimbalSystemDriver::driveRaw(int mode, int gas){
  if(mode == FORWARD){
    digitalWrite(ena, HIGH);
    analogWrite(in1, gas);
    analogWrite(in2, 0);
  }
  else if(mode == REVERSE){
    digitalWrite(ena, HIGH);
    analogWrite(in2, gas);
    analogWrite(in1, 0);

  }
  else if(mode == FASTSTOP){
    digitalWrite(ena, HIGH);
    analogWrite(in1, 0);
    analogWrite(in2, 0);
  }
  else if(mode == FREESTOP){
    digitalWrite(ena, LOW);
    analogWrite(in1, 0);
    analogWrite(in2, 0);  
  }
  return 0;
}

int GimbalSystemDriver::readPos(void){
  return (axis->read()+enc_offset);
}

int GimbalSystemDriver::findZero(void){
  driveRaw(FORWARD,150);
  delay(1000);
  int high = axis->read();
  //Serial.println(high);
  delay(250);
  driveRaw(REVERSE,150);
  delay(1000);
  int low = axis->read();
  //Serial.println(low);
  delay(250);
  driveRaw(FREESTOP,0);
  enc_offset = -(high + low)/2;
  //enc_offset = -low;
  return 0;
}

int GimbalSystemDriver::driveCurrent(float cur, float t){
  descur = cur;
  readCurrent_raw(); //Read Current Sensor
  nowerr_c = descur - current_mA_fil1; //Calculate Errors (Using filtered current data)
  deriv_c = 0;
  integ_c += nowerr_c*t;

  float p = nowerr_c * kp_cur; //Calculate Controller Output Terms
  float i = integ_c * ki_cur;
  float d = deriv_c * kd_cur; 

  if (nowerr_c == lasterr_c)      //If sign of error changes, zero integral term to prevent windup
    i = integ_c = 0;
  else if((nowerr_c > 0) && (lasterr_c < 0))
    i = integ_c = 0;
  else if((nowerr_c < 0) && (lasterr_c > 0))
    i = integ_c = 0;
  else if(i>integral_limit) //Enforce Limits for Integral Term to Prevent Windup
    i = integral_limit;
  else if(i<-integral_limit)
    i = -integral_limit;

  lasterr_c = nowerr_c;
  
  if ((descur > stall_thresh) || (descur < -stall_thresh))
    outp = kc_cur*(p + i + d);
  else
    outp=0;
  
  //float outp = 0;
  if(outp>100.0)
    outp=100.0;
  else if(outp<-100.0)
    outp=-100.0;
    
  if (outp>1){
    gas = int((outp*(255-motor_thresh)/100.0) + motor_thresh);
    driveRaw(FORWARD,gas); 
  }
  else if (outp<-1){
    outp = -outp;
    gas = int((outp*(255-motor_thresh)/100.0) + motor_thresh);
    //gas = 0;
    driveRaw(REVERSE,gas); 
  }
  else{
    driveRaw(FREESTOP,gas); 
  }
  return 0;
}
