#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int xAxisPin = A0;
const int yAxisPin = A1;

//vars for the joystick
int xAxisValue = 0;
int yAxisValue = 0;


//create an RF24 object
RF24 radio(9, 8);

//address through which two modules communicate.
const byte address[6] = "00001";

void setup()
{
  radio.begin();
  
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();
}
void loop()
{
  xAxisValue = analogRead(xAxisPin);
  yAxisValue = analogRead(yAxisPin);

  //Send message to receiver
  int vector[2]={0,0};
  vector[0] = xAxisValue;
  vector[1] = yAxisValue;
  radio.write(&vector, sizeof(vector));
}