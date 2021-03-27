//Here's the implementation

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

int GimbalSystemDriver::setMotorPins(int _ena int _in1, int _in2){
  ena = _ena;
  in1 = _in1;
  in2 = _in2;
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  return 0;
}

int GimbalSystemDriver::setMotorKs(float _kv){
  kv = _kv;
  km = 1 * kv;
  return 0;
}

const int GimbalSystemDriver::configurePins(void){
  
  return 0; 
}
