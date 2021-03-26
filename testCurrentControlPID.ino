#include "motorDriver.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219; //Create Current Sensor Object

motorDriver motor1; //Create motorDriver object

int i = 0;
elapsedMillis sinceRead; 
float curerr=0;
float lasterr=0;
float deriv=0; 
float integ=0;
float descur=200; //Our setpoint, in mA. "Maintain 200mA"
float outp=0;
int kp = 1;  //Proportional constant
int kd = 1;  //Derivative constant
int ki = 0;  //Integral constant

void setup() {
  Serial.begin(9600);
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
  int num2 = num2f;
  num2 = num2 % 256;
  Serial.print("Mode: ");
  Serial.println(num1);
  Serial.print("Speed: ");
  Serial.println(num2);
  motor1.spinMotor(num1,num2);
  */

  

  while (sinceRead < 100){} //This code acts like a timer/interrupt, waits 100ms to read current sensor, and drive motor
  sinceRead=0;              //Note: Minimum delay value is around ~65ms due to our sensor, in it's configuration rn

  
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  
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
  Serial.println(current_mA);

  curerr = descur - current_mA; //Calculate proportional error
  deriv = (curerr - lasterr)/(0.1); //Calculate derivative error
  integ += curerr/0.1;   //Calculate integral error
  outp = curerr*kp + deriv*kd + integ*ki; //Output is each error scaled by corresponding k
  
  lasterr = curerr; //Save error for next run for derivative term
  if (outp>0.0){ //If output positive, spin clockwise
    if (outp>100.0){
      outp=100.0;
    }
    motor1.spinMotor(0,outp);
  }
  else{  //Else spin counter clockwise
    outp=-outp;
    if (outp>100.0){
      outp=100.0;
    }
    motor1.spinMotor(1,outp);
  }
  //Maybe there should be a case for "coasting?", ie freestop mode
  
  Serial.print(",");   //We can print 2 values to serial monitor by doing this
  Serial.println(outp);
}
