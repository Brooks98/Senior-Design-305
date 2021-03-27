/* motorDriver.cpp
 * Header file for all of our controller's DC Motor needs
 * Team 305: Haptic Feedback Controller
 * FAMU-FSU College of Engineering
 */


// Define physical parameters of dc brushless motor
#define MOTOR_R 1
#define MOTOR_L 1
#define MOTOR_KV 1
#define MOTOR_KM 1
#define MOTOR_J 1

//L298
#define DEFAULT_ENA_PIN 9
#define DEFAULT_IN_PIN1 10
#define DEFAULT_IN_PIN2 11

//INA219
enum motormode {
  FORWARD=0,
  REVERSE=1,
  FASTSTOP=2,
  FREESTOP=3,
};
class motorDriver{
  public:
    motorDriver(); //Default constructor, use default motor parameters
    //motorDriver(float r_in,float l_in,float kv_in,float km_in,float j_in); //Constructor with motor parameters 
    void spinMotor(int mode, float pulse_per);
    void PinSetup(int ena, int in1, int in2);
    void setThreshold(int thresh);
    
    int ena_pin, in_pin1, in_pin2;
    int gas;
    float r, l; //resistance and inductance
    float kv, km; //voltage and torque (motor) constants
    float j; //Motor or system moment of inertia
    float w_est; //Estimated angular velocity
    int motor_thresh;
};


motorDriver::motorDriver(){
  ena_pin = DEFAULT_ENA_PIN;
  in_pin1 = DEFAULT_IN_PIN1;
  in_pin2 = DEFAULT_IN_PIN2;
  r = MOTOR_R;
  l = MOTOR_L;
  kv = MOTOR_KV;
  km = MOTOR_KM;
  j = MOTOR_J;
  gas = 0;
  w_est = 0;
  motor_thresh = 0;
}

void motorDriver::setThreshold(int thresh){
  motor_thresh = thresh;  
}

void motorDriver::spinMotor(int mode, float pulse_per){
  gas = int(pulse_per*255/100) % 256 ;
  if(mode == FORWARD){
    digitalWrite(ena_pin, HIGH);
    analogWrite(in_pin1, gas);
    analogWrite(in_pin2, 0);
  }
  else if(mode == REVERSE){
    digitalWrite(ena_pin, HIGH);
    analogWrite(in_pin1, 0);
    analogWrite(in_pin2, gas);
  }
  else if(mode == FASTSTOP){
    digitalWrite(ena_pin, HIGH);
    analogWrite(in_pin1, 0);
    analogWrite(in_pin2, 0);
  }
  else if(mode == FREESTOP){
    digitalWrite(ena_pin, LOW);
    analogWrite(in_pin1, 0);
    analogWrite(in_pin2, 0);  
  }
}

void motorDriver::PinSetup(int ena, int in1, int in2){
  ena_pin = ena;
  in_pin1 = in1;
  in_pin2 = in2;
  pinMode(ena_pin, OUTPUT);
  pinMode(in_pin1, OUTPUT);
  pinMode(in_pin2, OUTPUT);
}  
