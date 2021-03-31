//Here's the implementation


/*
 *  Default constructor, sets all things to default values
 */
GimbalSystemDriver::GimbalSystemDriver(){
  km = 1; kv = 1;
  ena = DEFAULT_MENA; in1 = DEFAULT_MIN1; in2 = DEFAULT_MIN2; //Use Pins 9,10,11 as default for motor
  enc1 = DEFAULT_ENC1; enc2 = DEFAULT_ENC2; //Set default Encoder Pins
  motor_thresh = DEFAULT_MOTOR_THRESH; 
  stall_thresh = DEFAULT_STALL_THRESH;
  refresh_period = DEFAULT_REFRESH_PERIOD; //100Hz operating frequency
  numsamples = DEFAULT_NUMSAMPLES; //8 Point MAF Default
  
  desspeed = descur = destorque = 0; //Zero setpoint
  nowerr_c = lasterr_c = deriv_c = integ_c = 0; //Zero PID error terms
  nowerr_s = lasterr_s = deriv_s = integ_s = 0;
  for (int i=0;i<MAX_SAMPLES;i++) //Zero MAF buffer
    currents[i] = speeds[i] = 0;

}

/*
 *  setMotorPins: configures pins for enable, input1, and input2. for L298N Inputs correspond to clockwise and counterclockwise.
 *  INPUTS _ena, _in1, _in2 - L298 Motor driver pins, _in1, and _in2 MUST be FlexPWM pins
 *  NOTES - MUST USE A FLEXPWM timer controlled pin! NOT a QuadTimer!
 */
int GimbalSystemDriver::setMotorPins(int _ena int _in1, int _in2){
  ena = _ena;
  in1 = _in1;
  in2 = _in2;
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
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
    analogWriteFrequency(in1, 585937.5);
    analogWriteFrequency(in1, 585937.5);  
    analogWriteResolution(8);
  }
  else if(_resPWM == 9){
    analogWriteFrequency(in1, 292968.75);
    analogWriteFrequency(in1, 292968.75);  
    analogWriteResolution(9);
  } 
  else if(_resPWM == 10){
    analogWriteFrequency(in1, 146484.38);
    analogWriteFrequency(in1, 146484.38);  
    analogWriteResolution(10);
  }
  else{
    return -1;
  }
  resPWM = _resPWM;
  return 0; 
}
