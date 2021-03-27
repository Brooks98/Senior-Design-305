//Here's the implementation

GimbalSystemDriver::GimbalSystemDriver(){
  km = 1; kv = 1;
  m_ena = 9; m_in1 = 10; m_in2 = 11; //Use Pins 9,10,11 as default
  motor_thresh = 0; stall_thresh = 0;
  refresh_period = 0.01; //100Hz operating frequency
  numsamples = 8; //8 Point MAF Default
  desspeed = 0; descur = 0; destorque = 0; //Zero setpoint
  nowerr_c = lasterr_c = deriv_c = integ_c = 0; //Zero PID error terms
  nowerr_s = lasterr_s = deriv_s = integ_s = 0;
  for (int i=0;i<MAX_SAMPLES;i++) //Zero MAF buffer
    currents[i] = speeds[i] = 0;
  
}

int GimbalSystemDriver::setMotorPins(int _ena int _in1, int _in2){
  ena = _ena;
  in1 = _in1;
  in2 = _in2;
  return 0;
}

int GimbalSystemDriver::setMotorKs(float _kv){
  kv = _kv;
  km = 1 * kv;
  return 0;
}
