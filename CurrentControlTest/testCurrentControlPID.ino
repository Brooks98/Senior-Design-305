#include "motorDriver.h" //Make sure to have this file and the .cpp in your project folder
#include <Wire.h>
#include "Adafruit_INA219.h" 
//Holy moley this is actually kinda working

Adafruit_INA219 ina219; //Create Current Sensor Object

motorDriver motor1; //Create motorDriver object

#define MAF_SAMPLES 16 
#define REFRESH_PERIOD 0.02 //In s. Corresponds to sampling rate of 50Hz
#define MOTOR_THRESHOLD 10 //Any output below this percent will have the motor free spin
elapsedMillis sinceRead; 
float curerr=0;
float lasterr=0;
float deriv=0; 
float integ=0;
float descur=50; //Our setpoint, in mA. "Maintain 200mA"
float outp=0;


float nowcur = 0;
float currents[MAF_SAMPLES];

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;

//PID Controller Constants
float kp = 0.10;  //Proportional constant
float kd = 0.00001;  //Derivative constant
float ki = 0.6;  //Integral constant
float kc = 1.0; //Overall Controller Gain



void setup() {
  Serial.begin(9600);
  for(int i=0;i<MAF_SAMPLES;i++){
    currents[i]=0;
  }
  motor1.PinSetup(9,10,11); //Setup motor driver with pin 9: ENA, pin 10: IN1, pin 11: IN2
  outp=60; //Initial motor output value (In percent)
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  motor1.spinMotor(0,outp); //Inital Spin
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
    nowcur=current_mA;
    for(int i=0; i<MAF_SAMPLES; i++)
      nowcur+=currents[i];
    
    nowcur = nowcur/(MAF_SAMPLES+1);
    
    for(int i = MAF_SAMPLES-1; i>0; i--)
      currents[i]=currents[i-1];
      
    currents[0]=current_mA;
    curerr = descur - nowcur; //Calculate proportional error
    deriv = (curerr - lasterr)/REFRESH_PERIOD; //Calculate derivative error
    integ += curerr*REFRESH_PERIOD;   //Calculate integral error
  
    outp = kc*(curerr*kp + deriv*kd + integ*ki); //Output is each error scaled by corresponding k
  
    lasterr = curerr; //Save error for next run for derivative term

    if (outp>100.0)
      outp=100.0;
    else if(outp<-100.0)
      outp=-100.0;
 
    //Serial.print(curerr);
    //Serial.print("  ");
    //Serial.print(deriv);
    //Serial.print("  ");
    //Serial.print(integ);
    //Serial.print("  ");
    Serial.print(nowcur);
    Serial.print("  ");
    Serial.println(outp);
    

    
    if (outp>(MOTOR_THRESHOLD)) //If output positive, spin clockwise
      motor1.spinMotor(FORWARD,outp);
    else if (outp<(-MOTOR_THRESHOLD)){  //If output negative, counter clockwise
      outp=-outp;
      motor1.spinMotor(REVERSE,outp);
    }
    else{ //Motor is in "dead zone", free stop
      motor1.spinMotor(FREESTOP,0);
    }

  }
}
