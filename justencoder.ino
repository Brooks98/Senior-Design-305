#include <Encoder.h>
Encoder motorleft(2, 3);
long positionLeft  = -999;

int in_pin1 = 5;
int in_pin2 = 4;
int enable_pin = 6;
int dir;

int forwardmax; 
int backwardmax;
int center;
int current = 999;

int distance = 0;

void setup() {
  Serial.begin(9600);
  

  initialvalues(); //calculates center encoder value at startup
}

void loop() {
  // encoder calculations
  long newLeft = 89;
  newLeft = motorleft.read();
  if (newLeft != current) {

    current = newLeft;
  }


  //Print Values for testing

  Serial.print("Left Encoder = ");
  Serial.print(current);
  Serial.print("\n");
  delay(500);


 // recenter();
  
  // if a character is sent from the serial monitor,
  // reset both back to zero.
//  if (Serial.available()) {
//    Serial.read();
//    Serial.println("Reset both knobs to zero");
//    motorleft.write(0);
//  }
}

void initialvalues(){

  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 255);
  analogWrite(in_pin2, 0);
  delay(1000);
  forwardmax = motorleft.read();

  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 0);
  analogWrite(in_pin2, 255);
  delay(1000);
  backwardmax = motorleft.read();

  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 0);
  analogWrite(in_pin2, 0);
  
  center = (forwardmax+backwardmax)/2;
  Serial.print("forwardmax = ");
  Serial.print(forwardmax);
  Serial.print("\n");
  Serial.print("backwardmax = ");
  Serial.print(backwardmax);
  Serial.print("\n");
  Serial.print("center = ");
  Serial.print(center);
  Serial.print("\n");
}
