#include "motorDriver.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

motorDriver motor1;
int i = 0;
int oldtime, newtime, delta;
elapsedMillis sinceRead;
float olderr=0; 
float lasterr=0; 
float curerr=0;
float deriv=0; 
float integ=0;
float descur=200; //ma
float outp=0;
int kp = 1;
int kd = 1;
int ki = 0;

void setup() {
  Serial.begin(9600);
  motor1.PinSetup(9,10,11);
  outp=60;
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  motor1.spinMotor(0,outp);
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

  

  while (sinceRead < 100){}
  sinceRead=0;
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

  curerr = descur - current_mA;
  deriv = (curerr - lasterr)/(0.1);
  integ += curerr/0.1;
  outp = curerr*kp + deriv*kd + integ*ki;
  if (outp>0.0){
    if (outp>100.0){
      outp=100.0;
    }
    motor1.spinMotor(0,outp);
  }
  else{
    outp=-outp;
    if (outp>100.0){
      outp=100.0;
    }
    motor1.spinMotor(1,outp);
  }
  Serial.print(",");
  Serial.println(outp);
}
