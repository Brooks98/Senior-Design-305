#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <NewPing.h>

#define trigPin_front  2
#define echoPin_front  2
#define trigPin_back  3
#define echoPin_back  3
#define MAX_DISTANCE 350 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define pingSpeed 100
double maxspeed = 512;

// Define variables for radio
RF24 radio(9, 10); // CE to pin D9, CSN to pin D10
const byte addresses [][6] = {"007", "001"};  //Setting the two addresses. One for transmitting and one for receiving

// declaring array for encoder values to recieve info
int data[2]={0,0};// data[0]=speed, data[1]= direction 

//servo
Servo dirServo;                   // define servo to control turning of smart car
int dirServoPin = 2;              // define pin for signal line of the last servo
float dirServoOffset = 6;         // define a variable for deviation(degree) of the servo
int turn;

const int dirAPin = 7;    // define pin used to control rotational direction of motor A
const int pwmAPin = 6;    // define pin for PWM used to control rotational speed of motor A
const int dirBPin = 4;    // define pin used to control rotational direction of motor B
const int pwmBPin = 5;    // define pin for PWM used to control rotational speed of motor B

#define FORWARD HIGH
#define BACKWARD LOW
 int speedy;
// int turn; 
 bool directionleft;

// Define variables for HC-SR04
unsigned long pingTimer_front, pingTimer_back;
NewPing sonar_front(trigPin_front, echoPin_front, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar_back(trigPin_back, echoPin_back, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

void setup() {
  // void setup for radio and HC-SR04
  pinMode(dirAPin, OUTPUT);   // set dirAPin to output mode
  pinMode(pwmAPin, OUTPUT);   // set pwmAPin to output mode
  pinMode(dirBPin, OUTPUT);   // set dirBPin to output mode
  pinMode(pwmBPin, OUTPUT);   // set pwmBPin to output mode
//  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
//  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  //servo for turning 
  dirServo.attach(dirServoPin);  // attaches the servo on servoDirPin to the servo object
  
  radio.startListening(); 
}

 

void loop()
{  
  // HC-SR04
  
  //delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  if (millis() >= pingTimer_front)
  {
    pingTimer_front += pingSpeed;
    int distance1 = sonar_front.ping_in();
    //duration = sonar.ping();
    //distance1 = (duration / 2) * 0.0343;
    Serial.print("Distance Front = ");
    Serial.print(distance1); // Distance will be 0 when out of set max range.
    Serial.println(" cm");
    delay(250);
  }

  if (millis() >= pingTimer_back)
  {
    pingTimer_back = pingTimer_front + (pingSpeed/2);
    int distance2 = sonar_back.ping_in();
    Serial.print("\t\t\tDistance Back = ");
    Serial.print(distance2); // Distance will be 0 when out of set max range.
    Serial.println(" cm");
    delay(250);
  }
  
  delay(500);
}


  //// Radio code
  delay(5);
  radio.stopListening();                             //This sets the module as transmitter

  radio.write(&distance, sizeof(distance)); // Sending data for the distance sensor
 
  delay(5);
  radio.startListening();   //This sets the module as receiver
  delay(5);
  if (radio.available())                     //Looking for incoming data
  { 
    //radio.read(&speedy, sizeof(speedy));
    //radio.read(&turn, sizeof(turn));
    radio.read(data, sizeof(data));
    speedy=data[0];
    turn=data[1]; 
    double speedy2 = speedy*1.0;
    Serial.print("Speed = ");
    Serial.print(speedy);
    Serial.print("\t");
    Serial.print("Speed2 = ");
    Serial.print(speedy2);
    Serial.print("\t");
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
    Serial.print("NewSpeed: ");
    Serial.print(newspeed);
    Serial.print("\t");
    
    // calculate the steering angle of servo according to the direction joystick of remote control and the deviation
    Serial.print("Data[1]: ");
    Serial.print(data[1]);
    Serial.print("\t");
    
    turn = constrain(turn, -50, 50);
    turn = map(turn, -50, 50, 135 ,45 ); 
    Serial.print("Servo degree: ");
    Serial.print(turn);
    Serial.print("\n");
    
    digitalWrite(dirAPin, directionleft);
    digitalWrite(dirBPin, directionleft);
    analogWrite(pwmAPin, newspeed);
    analogWrite(pwmBPin, newspeed);
    dirServo.write(turn + dirServoOffset);
  }
  
}
*/
