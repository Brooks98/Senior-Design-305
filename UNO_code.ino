/*Gyro Libraries*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*Radio Libraries*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

RF24 radio(9, 10); // CE, CSN
const byte addresses [][6] = {"00001", "00002"};  //Setting the two addresses. One for transmitting and one for receiving
int button_pin = 2;
int led_pin = 3;
boolean button_state = 0;
boolean button_state1 = 0;
float x_orientation = 0;
float y_orientation = 0;
float z_orientation = 0;

void setup() {
  /*LED Switch*/  
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver. 


  /*Gyroscope*/
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}

void loop() 
{  
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  delay(5);
  radio.stopListening();                             //This sets the module as transmitter
  
  /*LED Switch*/
  button_state = digitalRead(button_pin);
  radio.write(&button_state, sizeof(button_state));  //Sending button data

  /*Gyroscope*/
  x_orientation = digitalRead(event.orientation.x);
  radio.write(&x_orientation, sizeof(x_orientation));     //Sending x data
  
  y_orientation = digitalRead(event.orientation.y);
  radio.write(&y_orientation, sizeof(y_orientation));     //Sending y data

  z_orientation = digitalRead(event.orientation.z);
  radio.write(&z_orientation, sizeof(z_orientation));     //Sending z data 
  delay(5);

  
  radio.startListening();                            //This sets the module as receiver
  while(!radio.available());                         //Looking for incoming data
  radio.read(&button_state1, sizeof(button_state1)); //Reading the data
  if (button_state1 == HIGH)
  {
    digitalWrite(led_pin, HIGH);
  }
  else
  {
    digitalWrite(led_pin, LOW);
  }
}
