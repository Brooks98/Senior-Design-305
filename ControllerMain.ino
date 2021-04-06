//Encoder *********************************
#include <Encoder.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

Encoder motorleft(2, 3); //(2,3)
Encoder motorright(33,34);
long positionLeft  = -999;

int in_pin1 = 20;
int in_pin2 = 19;
int enable_pinL = 37;

int in_pin3 = 17;
int in_pin4 = 16;
int enable_pinR = 18;

int forwardmax;
int backwardmax;
int centerleft;
int currentleft;

int rightmax;
int leftmax;
int centerright;
int currentright;
//****************************************

//Radio *********************************
RF24 radio(8, 7); // CE, CSN
const byte addresses [][6] = {"007", "001"};    //Setting the two addresses. One for transmitting and one for receiving
int distance = 0;
//***************************************

//Info to send to vehicle ******************
int data[4] = {0,0,0,0}; //data[0] = speed, data[1] = turning
int speedy; //negative is backwards, forward in positive
int turn;
//******************************************


void setup() {
  Serial.begin(9600);
  Serial.println("Setup");

  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX);            //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  //radio.setDataRate( RF24_250KBPS );
  //radio.setRetries(1,15); // delay, count
  //initialvalues(); //calculates center encoder value at startup
  //recenter();
  
  pinMode(enable_pinL, OUTPUT);
  pinMode(enable_pinR, OUTPUT);
  pinMode(in_pin1, OUTPUT);
  pinMode(in_pin2, OUTPUT);
  pinMode(in_pin3, OUTPUT);
  pinMode(in_pin4, OUTPUT);
 
  
}
void loop()
{ 
  
  centerleft = 0;
  forwardmax = 50;
  backwardmax = -50;
//  delay(100);
  // Gets encoder values **********
  int newLeft;
  newLeft = motorleft.read();
  if (newLeft != currentleft) {

    currentleft = newLeft;
  }

  int newRight;
  newRight = motorright.read();
  if(newRight != currentright)
  {
    currentright = newRight;
  }

  //*****************************

  //Calculates speed & direction ********
  //speedy = (currentleft - centerleft)* ((200)/(abs(forwardmax)+abs(backwardmax)));
  speedy = currentleft;
  data[1] = speedy;
  turn = currentright;
  data[0] = turn;
  //*************************************
 
  delay(50);// increase maybe? 
  radio.stopListening(); 
  radio.write(data, sizeof(data));
  delay(50); 
  radio.startListening();                    //This sets the module as receiver
 
  if (radio.available())                     //Looking for incoming data
  {

    Serial.print("Left Encoder = ");
    Serial.print(currentleft);
    Serial.print("\tSpeedy = ");
    Serial.print(speedy);
    Serial.print("\tRight Encoder = ");
    Serial.print(currentright);
    Serial.print("\t");
    
    radio.read(&distance, sizeof(distance)); // reading in data for distance sensor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
//
//    radio.stopListening();
//
//    radio.write(data, sizeof(data));
    //radio.write(&turn, sizeof(turn));
    //Feedback ****
//    while(current > center + 5 && distance < 20)
//    {
//      newLeft = motorleft.read();
//      if (newLeft != current) {
//        current = newLeft;
//      }
//      delay(10);
//      radio.startListening();
//      radio.read(&distance, sizeof(distance)); // reading in data for distance sensor
//      Serial.print("Distance: ");
//      Serial.print(distance);
//  //    Serial.println(" cm");
//      Serial.print("cm\tLeft Encoder = ");
//      Serial.print(current);
//      Serial.print("\n");
//      digitalWrite(enable_pin, HIGH);
//     analogWrite(in_pin1, 150);
//     analogWrite(in_pin2, 0);
//    
////     digitalWrite(enable_pin, HIGH);
////     analogWrite(in_pin1, 0);
////     analogWrite(in_pin2, 0);
//      radio.stopListening();  
//    }
//    
//     digitalWrite(enable_pin, HIGH);
//     analogWrite(in_pin1, 0);
//     analogWrite(in_pin2, 0);
 
//    delay(5);
//    radio.startListening();
   
 
  }
//  else
//  {
////    delay(1000);
////    Serial.print("Radio Not reading\n");
//  }

   
//**********************************************************
 if(currentleft > (centerleft + 5) && (distance < 20))
    {
      Serial.print("in if\n");
      int joystickspeed;
      if (distance > 3)
      {
        joystickspeed = map(distance, 0, 20, 200, 175);
      }
      else
      {
        joystickspeed = 200;
      }

      leftbackward(joystickspeed);
       
    }
    else
    {
      digitalWrite(in_pin1, LOW);
      digitalWrite(in_pin2, LOW);
//      digitalWrite(in_pin3, LOW);
//      digitalWrite(in_pin4, LOW);
    }
 
//*********************************************************
   //radio.stopListening();                           //This sets the module as transmitter
    //recenter();
}

void initialvalues(){

  leftbackward(255);
  delay(1000);
 
  leftforward(255);
  delay(1000);
  forwardmax = motorleft.read();
  delay(300);
 
  leftbackward(255);
  delay(1000);
  backwardmax = motorleft.read();
  delay(300);

  leftstop();
 
  centerleft = (forwardmax+backwardmax)/2;
  Serial.print("forwardmax = ");
  Serial.print(forwardmax);
  Serial.print("\n");
  Serial.print("backwardmax = ");
  Serial.print(backwardmax);
  Serial.print("\n");
  Serial.print("center = ");
  Serial.print(centerleft);
  Serial.print("\n");
}
void leftforward(int sp)
{
  analogWrite(enable_pinL, sp);
  digitalWrite(in_pin1, HIGH);
  digitalWrite(in_pin2, 0);
}
void leftbackward(int sp)
{
  analogWrite(enable_pinL, sp);
  digitalWrite(in_pin1, 0);
  digitalWrite(in_pin2, HIGH);
}
void leftstop()
{
  analogWrite(enable_pinL, 0);
  digitalWrite(in_pin1, 0);
  digitalWrite(in_pin2, 0);
}
void recenter(){
  while (currentleft < centerleft - 5 || currentleft > centerleft + 5)
  {
    delay(10);
    if (currentleft < centerleft - 5)
    {
      leftforward(150);
      long newLeft;
      newLeft = motorleft.read();
   
      if (newLeft != currentleft) {
        Serial.print("Left = ");
        Serial.print(newLeft);
        Serial.print("\n");
   
        currentleft = newLeft;
      }
    }
    if (currentleft > centerleft + 5)
    {
       leftbackward(150);
      long newLeft;
      newLeft = motorleft.read();
   
      if (newLeft != currentleft) {
        Serial.print("Left = ");
        Serial.print(newLeft);
        Serial.print("\n");
   
        currentleft = newLeft;
    }
  }
  }
   leftstop();

   
//  while(current > center + 5)
//  {
//    digitalWrite(enable_pin, HIGH);
//    analogWrite(in_pin1, 150);
//    analogWrite(in_pin2, 0);
//    long newLeft;
//    newLeft = motorleft.read();
//  
//    if (newLeft != current) {
//      Serial.print("Left = ");
//      Serial.print(newLeft);
//      Serial.print("\n");
//  
//      current = newLeft;
////       digitalWrite(enable_pin, HIGH);
////    analogWrite(in_pin1, 0);
////    analogWrite(in_pin2, 0);
////      delay(50);
//    }
//  }
//   digitalWrite(enable_pin, HIGH);
//    analogWrite(in_pin1, 0);
//    analogWrite(in_pin2, 0);
 
}
