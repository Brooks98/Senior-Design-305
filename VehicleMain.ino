/// New code for UNO w car
// This code doesnt do anything other than read in distance i.e. doesnt drive car
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define echoPin 4 // HC-SR04 echo pin to D4
#define trigPin 5 //HC-SR04 trig pin to D5 (PWM)
#define maxspeed 255

// Define variables for radio
RF24 radio(9, 10); // CE to pin D9, CSN to pin D10
const byte addresses [][6] = {"00001", "00002"};  //Setting the two addresses. One for transmitting and one for receiving
int button_pin = 2;
int led_pin = 3;
boolean button_state = 0;
boolean button_state1 = 0;
// Define variables for HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

const int dirAPin = 7;    // define pin used to control rotational direction of motor A
const int pwmAPin = 6;    // define pin for PWM used to control rotational speed of motor A
const int dirBPin = 4;    // define pin used to control rotational direction of motor B
const int pwmBPin = 5;    // define pin for PWM used to control rotational speed of motor B

#define FORWARD LOW
#define BACKWARD HIGH
 int speed;

void setup() {
  // void setup for radio and HC-SR04
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
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
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");


  //// Radio code
  delay(5);
  radio.stopListening();                             //This sets the module as transmitter

  radio.write(&distance, sizeof(distance)); // Sending data for the distance sensor
 
  delay(5);
  radio.startListening();   //This sets the module as receiver
  delay(5);
  if (radio.available())                     //Looking for incoming data
  { 
    radio.read(&speed, sizeof(speed));
    Serial.print("Speed = ");
    Serial.print(speed);
    Serial.print("\n");
    
    if(speed < 0)
    {
      digitalWrite(dirAPin, FORWARD);
      digitalWrite(dirBPin, FORWARD);
//      analogWrite(pwmAPin, (speed*maxspeed)/100);
//      analogWrite(pwmBPin, (speed*maxspeed)/100);
       analogWrite(pwmAPin, speed);
      analogWrite(pwmBPin, speed);
    }
    else if(speed > 0)
    {
      digitalWrite(dirAPin, BACKWARD);
      digitalWrite(dirBPin, BACKWARD);
      analogWrite(pwmAPin, -1*speed);
      analogWrite(pwmBPin, -1*speed);
    }
    else
    {
      //idk if we need this tbh
    }
  }
  
}
