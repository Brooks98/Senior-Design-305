  
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#define echoPin 0 // HC-SR04 echo pin to D4 
#define trigPin 1 //HC-SR04 trig pin to D5 (PWM)
double maxspeed = 512;

// Define variables for radio
RF24 radio(9, 10); // CE to pin D9, CSN to pin D10
const byte addresses [][6] = {"007", "001"};  //Setting the two addresses. One for transmitting and one for receiving

// declaring array for encoder values to recieve info
int data[4]={0,0,0,0};// data[0]=speed, data[1]= direction

//servo
Servo dirServo;                   // define servo to control turning of smart car
int dirServoPin = 2;              // define pin for signal line of the last servo
float dirServoOffset = -12;         // define a variable for deviation(degree) of the servo
int turn = 0;

// Define variables for HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

const int dirAPin = 7;    // define pin used to control rotational direction of motor A
const int pwmAPin = 6;    // define pin for PWM used to control rotational speed of motor A
const int dirBPin = 4;    // define pin used to control rotational direction of motor B
const int pwmBPin = 5;    // define pin for PWM used to control rotational speed of motor B
const int RPin = A3;
const int GPin = A4;
const int BPin = A5;
int RGBVal = 0;

#define FORWARD HIGH
#define BACKWARD LOW
 int speedy;
// int turn;
 bool directionleft;

void setup() {
  // void setup for radio and HC-SR04
  pinMode(dirAPin, OUTPUT);   // set dirAPin to output mode
  pinMode(pwmAPin, OUTPUT);   // set pwmAPin to output mode
  pinMode(dirBPin, OUTPUT);   // set dirBPin to output mode
  pinMode(pwmBPin, OUTPUT);   // set pwmBPin to output mode
  pinMode(RPin, OUTPUT);   // set RPin to output mode
  pinMode(GPin, OUTPUT);   // set GPin to output mode
  pinMode(BPin, OUTPUT);   // set BPin to output mode
 
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
 pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
//  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
//  Serial.println("vehicle code startup"); // print some text in Serial Monitor
// 
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  //radio.setDataRate( RF24_250KBPS );
//  radio.setRetries(0, 15);  
  radio.setRetries(1, 15);  
  //servo for turning
  dirServo.attach(dirServoPin);  // attaches the servo on servoDirPin to the servo object
 
  radio.startListening();
 
}

 

void loop()
{  // HC-SR04
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
//  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
//  // Displays the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm\t");


  //// Radio code
  delay(10);
  radio.startListening();   //This sets the module as receiver
  //delay(5);
  if (radio.available())                     //Looking for incoming data
  {
    digitalWrite(RPin, LOW);digitalWrite(GPin, HIGH);digitalWrite(BPin, LOW); //pretty colors
    //radio.read(&speedy, sizeof(speedy));
    //radio.read(&turn, sizeof(turn));
    radio.read(data, sizeof(data));
    speedy=data[2];
    turn=data[0];
    double speedy2 = speedy*1.0;
//    Serial.print("Speed = ");
//    Serial.print(speedy);
//    Serial.print("\t");
//    Serial.print("Speed2 = ");
//    Serial.print(speedy2);
//    Serial.print("\t");
    if(speedy >= 0)
    {
      directionleft = FORWARD;
    }
    else
    {
      directionleft = BACKWARD;
    }
    double newspeed = 2.0*(speedy2*maxspeed)/100.0;
    newspeed = abs(constrain(newspeed, -512, 512));
    newspeed = map(newspeed, 0, 512, 0, 255);
//    Serial.print("NewSpeed: ");
//    Serial.print(newspeed);
//    Serial.print("\t");
   
    // calculate the steering angle of servo according to the direction joystick of remote control and the deviation
//    Serial.print("Data[0]: ");
//    Serial.print(data[0]);
//    Serial.print("\t");
//    Serial.print("Data[2]: ");
//    Serial.print(data[2]);
//    Serial.print("\n");

   
    turn = constrain(turn, -50, 50);
    turn = map(turn, -50, 50, 45 ,135 );
//    Serial.print("Servo degree: ");
//    Serial.print(turn);
//    Serial.print("\n");
   
    digitalWrite(dirAPin, directionleft);
    digitalWrite(dirBPin, directionleft);
    analogWrite(pwmAPin, newspeed);
    analogWrite(pwmBPin, newspeed);
    dirServo.write(turn + dirServoOffset);
  }

  //delay(5);
  delay(10); 
  radio.stopListening();                             //This sets the module as transmitter

  radio.write(&distance, sizeof(distance)); // Sending data for the distance sensor
 
  //delay(5);
}
