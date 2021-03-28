#include "motorDriver.h" //Make sure to have this file and the .cpp in your project folder
#include <Wire.h>
#include "Adafruit_INA219.h" 
//Holy moley this is actually kinda working

Adafruit_INA219 ina219; //Create Current Sensor Object

motorDriver motor1; //Create motorDriver object

//#define MAF_SAMPLES 79 //This * Refresh_Period should be a multiple of our PWM period, 0.02s
#define REFRESH_PERIOD 0.005 //In s. Corresponds to sampling rate of 200Hz
#define MOTOR_THRESHOLD 3 //Any output below this percent will have the motor free spin
#define STALL_THRESHOLD 65

elapsedMillis sinceRead; 
float curerr=0;
float lasterr=0;
float deriv=0; 
float integ=0;
float descur=200; //Our setpoint, in mA. "Maintain 200mA"
float outp=0;

float lastcur1=0;
float lastcur2=0;
float nowcur = 0;
//float currents[MAF_SAMPLES];
//float smooth_currents[MAF_SAMPLES*4];


//IIR Filter Constants
//I tried a Moving Average Filter first, but IIR low pas filters work much better*
//a1 corresponds to the main 2 pole filter, a2 corresponds to a 1 pole filter used to smooth data for visualization, but responds slower to transients
float a1=0.815;
float a2=0.925;
float b1, b2;

float smoothnowcur = 0;
float smoothlastcur = 0;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

//PID Controller Constants
float kp = 0.15;  //Proportional constant
float kd = 0.0075;  //Derivative constant
float ki = 1.4;  //Integral constant
float kc = 1; //Overall Controller Gain



void setup() {
  Serial.begin(9600);
  //analogWriteFrequency(10, 2048);
  //analogWriteFrequency(11, 2028);
  //analogWriteResolution(9);
  //for(int i=0;i<MAF_SAMPLES;i++){
  //  currents[i]=0;
  //}
  motor1.PinSetup(9,10,11); //Setup motor driver with pin 9: ENA, pin 10: IN1, pin 11: IN2
  outp=60; //Initial motor output value (In percent)
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  ina219.setCalibration_32V_1A();
  motor1.spinMotor(0,outp); //Inital Spin
  motor1.setThreshold(STALL_THRESHOLD); //Set this, it helps a ton, take the slack out of motor
  b1 = 1 - a1; //IIR 2 Pole Constants
  b2 = 1 - a2; //2nd IIR 1 Pole Constants
}

void loop() {
  /*
  i=i%256;
  i++;
  Serial.println("Enter Vals");
  while (Serial.available()==0){
    }
  float num1f = Serial.parseFloat();
  float num2f = Serial.parseFloat();
  int num1 = num1f;
  int num2 s= num2f;
  num2 = num2 % 256;
  Serial.print("Mode: ");
  Serial.println(num1);
  Serial.print("Speed: ");
  Serial.println(num2);
  motor1.spinMotor(num1,num2);
  */
 
  if (sinceRead >= REFRESH_PERIOD*1000){ //Read values when timer reaches 100ms, then reset timer to zero
    sinceRead=0;
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    power_mW = ina219.getPower_mW();
    loadvoltage = busvoltage + (shuntvoltage / 1000);
  
    //Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    //Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    //Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
    //Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    //Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    //nowcur=current_mA;
    //for(int i=0; i<MAF_SAMPLES; i++)
    //  nowcur+=currents[i];
    
    //nowcur = nowcur/(MAF_SAMPLES+1);
    
    //for(int i = MAF_SAMPLES-1; i>0; i--)
    //  currents[i]=currents[i-1];
      
    //currents[0]=current_mA;
    nowcur = b1*current_mA + a1*lastcur1; //This is the initial 2 pole IIR filter
    lastcur1 = nowcur;
    nowcur = b1*nowcur + a1*lastcur2; 

    //deriv = (nowcur - lastcur2)/REFRESH_PERIOD;
    
    lastcur2 = nowcur;
    
    smoothnowcur = b2*nowcur + a2*smoothlastcur; //Second 1 pole IIR filter
    
    //Calculating our PID terms
    curerr = descur - nowcur; //Calculate proportional error
    //curerr = nowcur - descur; //If we're too high its a positive error

    //deriv = nowcur - descur; 
    deriv = (smoothnowcur - smoothlastcur)/REFRESH_PERIOD; //Calculate derivative error
    
    integ += curerr*REFRESH_PERIOD;   //Calculate integral error

    //Combining PID terms multiplied by PID parameters  
    outp = kc*(curerr*kp + deriv*kd + integ*ki); //Output is each error scaled by corresponding k
  
    lasterr = curerr; //Save error for next run for derivative term
    smoothlastcur = smoothnowcur;
    //Scale to 100% Can maybe do this more sophisticatedly, with exponential "soft" stops
    if (outp>100.0)
      outp=100.0;
    else if(outp<-100.0)
      outp=-100.0;

    //Use these to print parameters to Arduino Serial Plotter 
    Serial.print(curerr*kp);
    Serial.print("  ");
    Serial.print(deriv*kd);
    Serial.print("  ");
    Serial.print(integ*ki);
    Serial.print("  ");
    //Serial.print(current_mA);
    //Serial.print("  ");
    Serial.print(nowcur);
    Serial.print("  ");
    Serial.print(smoothnowcur);
    Serial.print("  ");
    Serial.println(outp);
   
    
    if (outp>(MOTOR_THRESHOLD)) //If output positive, spin clockwise
      motor1.spinMotor(FORWARD,outp);
    else if (outp<(-MOTOR_THRESHOLD)){  //If output negative, counter clockwise
      outp=-outp;//Make outp positive
      motor1.spinMotor(REVERSE,outp);
    }
    else{ //Motor is in "dead zone", free stop
      motor1.spinMotor(FREESTOP,0);
    }

  }
}
