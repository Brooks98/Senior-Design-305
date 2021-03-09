#include "motorDriver.h"

motorDriver motor1;
int i = 0;
void setup() {
  Serial.begin(9600);
  motor1.PinSetup(9,10,11);
}

void loop() {
  i=i%256;
  i++;
  Serial.println("Enter Vals");
  while (Serial.available()==0){}
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
}
