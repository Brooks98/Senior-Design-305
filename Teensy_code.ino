/*Gyro Libraries*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*Radio Libraries*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving
int button_pin = 8;
boolean button_state = 0;
boolean button_state1 = 0;
float x_orientation = 0;
float y_orientation = 0;
float z_orientation = 0;
int led_pin = 6;
void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX);            //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
}
void loop()
{
  delay(5);
  radio.startListening();                    //This sets the module as receiver
  if (radio.available())                     //Looking for incoming data
  {
    /*LED Switch*/
    radio.read(&button_state, sizeof(button_state));
    if(button_state == HIGH)
    {
      digitalWrite(led_pin, HIGH);
    }
    else
    {
      digitalWrite(led_pin, LOW);
    }

    /*Gyroscope*/
    radio.read(&x_orientation, sizeof(x_orientation));
    Serial.print("X: ");
    Serial.print(x_orientation, 4);
    delay(20);

    radio.read(&y_orientation, sizeof(y_orientation));
    Serial.print("\tY: ");
    Serial.print(y_orientation, 4);
    delay(20);

    radio.read(&z_orientation, sizeof(z_orientation));
    Serial.print("\tZ: ");
    Serial.print(z_orientation, 4);
    Serial.println("");
    delay(20);
  
    delay(5);

    /*LED Switch*/
    radio.stopListening();                           //This sets the module as transmitter
    button_state1 = digitalRead(button_pin);
    radio.write(&button_state1, sizeof(button_state1));   //Sending the data


  }
}
