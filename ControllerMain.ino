//Encoder *********************************
#include <Encoder.h>

Encoder motorleft(2, 3);
long positionLeft  = -999;

int in_pin1 = 20;
int in_pin2 = 19;
int enable_pin = 21;
int dir;

int forwardmax; 
int backwardmax;
int center;
int current;
//****************************************

//Radio *********************************
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 7); // CE, CSN
const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving
//int button_pin = 8;
//boolean button_state = 0;
//boolean button_state1 = 0;
//int led_pin = 6;
int distance = 0;
//***************************************

//Info to send to vehicle ******************
int speed; //negative is backwards, forward in positive

//******************************************


void setup() {
  Serial.begin(9600);
  Serial.println("Setup");
  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX);            //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  
  //initialvalues(); //calculates center encoder value at startup
  //recenter();
}
void loop()
{
  center = 0;
  forwardmax = 50;
  backwardmax = -50;
//  delay(100);
  // Gets encoder value ********** eventually add other motor
  long newLeft;
  newLeft = motorleft.read();
  if (newLeft != current) {

    current = newLeft;
  }
//  leftbackward();
//  delay(50);
  leftstop();
  //*****************************

  //Calculates speed & direction ********
  speed = (current - center)* ((200)/(abs(forwardmax)+abs(backwardmax)));
  //*************************************
  
  delay(10);
  radio.startListening();                    //This sets the module as receiver
 
  if (radio.available())                     //Looking for incoming data
  { //radio.read(&button_state, sizeof(button_state));
    radio.read(&distance, sizeof(distance)); // reading in data for distance sensor
    Serial.print("Distance: ");
    Serial.print(distance);
//    Serial.println(" cm");
    Serial.print("cm\tLeft Encoder = ");
    Serial.print(current);
    Serial.print("\tSpeed = ");
    Serial.print(speed);
    Serial.print("\n");

    radio.stopListening();
    radio.write(&speed, sizeof(speed));
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
 
   delay(5);
//
   radio.stopListening();                           //This sets the module as transmitter
    //recenter();
  }
}

void initialvalues(){

  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 255);
  analogWrite(in_pin2, 0);
  delay(2000);
  forwardmax = motorleft.read();

  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 0);
  analogWrite(in_pin2, 255);
  delay(2000);
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
void leftforward()
{
  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 255);
  analogWrite(in_pin2, 0);
}
void leftbackward()
{
  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 0);
  analogWrite(in_pin2, 255);
}
void leftstop()
{
  digitalWrite(enable_pin, HIGH);
  analogWrite(in_pin1, 0);
  analogWrite(in_pin2, 0);
}
void recenter(){
  while (current < center - 5 || current > center + 5) 
  {
    delay(10);
    if (current < center - 5)
    {
      digitalWrite(enable_pin, HIGH);
      analogWrite(in_pin1, 0);
      analogWrite(in_pin2, 150);
      long newLeft;
      newLeft = motorleft.read();
    
      if (newLeft != current) {
        Serial.print("Left = ");
        Serial.print(newLeft);
        Serial.print("\n");
    
        current = newLeft;
      }
    }
    if (current > center + 5)
    {
       digitalWrite(enable_pin, HIGH);
      analogWrite(in_pin1, 150);
      analogWrite(in_pin2, 0);
      long newLeft;
      newLeft = motorleft.read();
    
      if (newLeft != current) {
        Serial.print("Left = ");
        Serial.print(newLeft);
        Serial.print("\n");
    
        current = newLeft;
    }
  }
  }
   digitalWrite(enable_pin, HIGH);
    analogWrite(in_pin1, 0);
    analogWrite(in_pin2, 0);

    
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
