#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//motor pins for L293D chip
const int enablePin1 = 11;
const int in1Pin = 10;
const int in2Pin = 9;
const int enablePin2 = 6;
const int in3Pin = 5;
const int in4Pin = 3;

//vars for joystick
int xValue = 0;
int yValue = 0;

//buzzer
const int buzzerPin = 45;
const int buzzerTone = 1300;
long lastTimeSong = 0;
long diffTrig = 250;
boolean sing = false;

//servo, ultrasonic sensor and LEDs
const int trigPinFront = 30;
const int echoPinFront = 13;
const int trigPinBehind = 36;
const int echoPinBehind = 4;
const int rightRedLedPin = 40;
const int leftRedLedPin = 42;
const int rightYellowLedPin = 31;
const int leftYellowLedPin = 33;
const long triggerTimeAngle = 20;
const long triggerTimeUltraLow = 2;
const long triggerTimeUltraHigh = 1;

int servoPin = 26;
int trigState = LOW;
int printState = LOW;

long duration;
long distance;
long lastTimeChangeAngle = 0;
long previousTimeUltrasonic = 0;

boolean reverse = false;
Servo servo;

//vars for blinking LEDs
unsigned long ledDelay = 300;
unsigned long lastBlinkLed = 0;
unsigned long lastBlinkLedRight = 0;
boolean isLedOn = false;
boolean isLedRightOn = false;

//vars for ultrasonic sensors
int angle = 0;   // servo position in degrees
long frontDistance = 0;
long behindDistance = 0;

//create an RF24 object
RF24 radio(7, 8);

//address through which two modules communicate.
const byte address[6] = "00001";

//right motor
void setMotor1(int motorSpeed, boolean reverse)
{
  analogWrite(enablePin1, motorSpeed);
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);
}

//left motor
void setMotor2(int motorSpeed, boolean reverse)
{
  analogWrite(enablePin2, motorSpeed);
  digitalWrite(in3Pin, !reverse);
  digitalWrite(in4Pin, reverse);
}

//debugging function
void printDistance(long distanta) {
  Serial.print(distanta);
  Serial.println("  cm");
}

//calculate the distance for the ultrasonic sensors
long findDistance(const int trigPin, const int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;
  return distance;
}

void setup()
{
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin2, OUTPUT);

  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinBehind, OUTPUT);
  pinMode(echoPinBehind, INPUT);

  pinMode(rightRedLedPin, OUTPUT);
  pinMode(leftRedLedPin, OUTPUT);
  pinMode(rightYellowLedPin, OUTPUT);
  pinMode(leftYellowLedPin, OUTPUT);

  servo.attach(servoPin);

  while (!Serial);
  Serial.begin(9600);
  radio.begin();

  //set the address
  radio.openReadingPipe(0, address);

  //Set module as receiver
  radio.startListening();
}

void loop()
{
  int mapValue = 0;

  //movement for the radar
  if (angle <= 80 && reverse == false && millis() - lastTimeChangeAngle >= triggerTimeAngle)
  {

    servo.write(angle);
    lastTimeChangeAngle = millis();
    angle++;
    if (angle == 80)
    {
      reverse = true;
    }
  }

  if (angle >= 20 && reverse == true && millis() - lastTimeChangeAngle >= triggerTimeAngle)
  {
    servo.write(angle);
    lastTimeChangeAngle = millis();
    angle--;
    if (angle == 20)
    {
      reverse = false;
    }
  }

  //calculating the distance for the front and back ultrasonic sensors
  frontDistance = findDistance(trigPinFront, echoPinFront);
  behindDistance = findDistance(trigPinBehind, echoPinBehind);

  if (radio.available())
  {
    int vector[2];
    radio.read(&vector, sizeof(vector));
    xValue = vector[0];
    yValue = vector[1];

    //checking the distance from the two sensors and powering off the motors if necessary
    if (frontDistance <= 20)
    {
      setMotor1(0, false);
      setMotor2(0, false);
    }

    if (behindDistance <= 20)
    {
      setMotor1(0, false);
      setMotor2(0, false);
    }
    
    //backwards move
    if (behindDistance > 20 && xValue < 490 && yValue >= 515 && yValue <= 545)
    {
      
      if (millis() - lastTimeSong >= diffTrig && sing == false)
      {
        digitalWrite(rightRedLedPin, HIGH);
        digitalWrite(leftRedLedPin, HIGH);
        tone(buzzerPin, buzzerTone);
        lastTimeSong = millis();
        sing = true;
      }
      if (millis() - lastTimeSong >= diffTrig && sing == true)
      {
        digitalWrite(rightRedLedPin, LOW);
        digitalWrite(leftRedLedPin, LOW);
        noTone(buzzerPin);
        lastTimeSong = millis();
        sing = false;
      }
      
      mapValue = map(xValue, 489, 0, 0, 240);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
      setMotor1(mapValue, false);
      setMotor2(mapValue, false);
    }
    
    //forward move
    if (frontDistance > 20 && xValue > 510 && yValue >= 515 && yValue <= 545)
    {
      mapValue = map(xValue, 511, 1023, 0, 255);
      int mapValue1 = map(xValue, 511, 1023, 0, 195); 
      setMotor1(mapValue, true);
      setMotor2(mapValue1, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }

    //left move
    if (yValue < 515 && xValue >= 490 && xValue <= 510)
    {
      if (millis() - lastBlinkLed >= ledDelay && isLedOn == false)
      {
        digitalWrite(rightYellowLedPin, LOW);
        digitalWrite(leftYellowLedPin, HIGH);
        lastBlinkLed= millis();
        isLedOn = true;
      }
      if (millis() - lastBlinkLed >= ledDelay && isLedOn == true)
      {
        digitalWrite(leftYellowLedPin, LOW);
        lastBlinkLed= millis();
        isLedOn = false;
      }

      mapValue = map(yValue, 514, 0, 0, 200);
      setMotor1(mapValue, true);
      setMotor2(0, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
    }

    //right move
    if (yValue > 545 && xValue >= 490 && xValue <= 510)
    {
      if (millis() - lastBlinkLedRight >= ledDelay && !isLedRightOn)
      {
        digitalWrite(rightYellowLedPin, HIGH);
        digitalWrite(leftYellowLedPin, LOW);
        lastBlinkLedRight = millis();
        isLedRightOn = true;
      }
      if (millis() - lastBlinkLedRight >= ledDelay && isLedRightOn)
      {
        digitalWrite(rightYellowLedPin, LOW);
        digitalWrite(leftYellowLedPin, LOW);
        lastBlinkLedRight = millis();
        isLedRightOn = false;
      }

      mapValue = map(yValue, 546, 1023, 0, 255);
      setMotor1(0, true);
      setMotor2(mapValue, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
    }

    //stationary
    if (xValue >= 490 && xValue <= 510 && yValue >= 515 && yValue <= 545)
    {
      setMotor1(0, true);
      setMotor2(0, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }

    //forward right oblique
    if (frontDistance > 30 && xValue >= 850 && xValue <= 1023 && yValue >= 850 && yValue <= 1023)
    {
      setMotor1(130, true);
      setMotor2(230, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }

    //forward left oblique
    if (frontDistance > 30 && xValue >= 850 && xValue <= 1023 && yValue <= 200)
    {
      setMotor1(230, true);
      setMotor2(130, true);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }

    //backwards right oblique
    if (yValue >= 850 && yValue <= 1023 && xValue <= 200)
    {
      setMotor1(70, false);
      setMotor2(120, false);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }

    //backwards left oblique
    if (yValue <= 200 && xValue <= 200)
    {
      setMotor1(120, false);
      setMotor2(70, false);
      noTone(buzzerPin);
      digitalWrite(rightRedLedPin, LOW);
      digitalWrite(leftRedLedPin, LOW);
      digitalWrite(rightYellowLedPin, LOW);
      digitalWrite(leftYellowLedPin, LOW);
    }
  }
}