#include <Arduino.h>

const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

const int pinSW = A2;
const int pinX = A0;
const int pinY = A1; 

int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;
int digitsValues[noOfDigits] = {0, 0, 0 ,0};
int dpsStates[noOfDigits] = {1, 1, 1, 1};
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDellay = 50;
int buttonState = 0;
int dpState = LOW;
int currentDisplay = 0;
int minThreshold = 400;
int maxThreshold = 600;
bool isXJoyMoved = false;
bool isYJoyMoved = false;


// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
};

byte digitMatrix[noOfDigits][segSize - 1] = {
  // a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void displayNumber(byte digit = 0, int decimalPoint = HIGH) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  if (decimalPoint == LOW) {
    analogWrite(segments[segSize - 1], 1023);

  }
  else {
    analogWrite(segments[segSize - 1], 0);
  }
}

void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}


void setup() {
  displayNumber();
  for (int i = 0; i < segSize - 1; i++)
  {
    pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < noOfDisplays; i++)
  {
    pinMode(digits[i], OUTPUT);
  }
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);
}

void loop() {
  yAxisValue = analogRead(pinY);

  int reading = digitalRead(pinSW);
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDellay) {

    if (reading !=  buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        dpState = !dpState;
        dpsStates[currentDisplay] = !dpState;
      }
    }
  }
  lastButtonState = reading;

  if (dpState == false && yAxisValue < minThreshold && !isYJoyMoved) {
    if (currentDisplay < 3) {
      currentDisplay++;
    } else {
      currentDisplay = 0;
    }
    isYJoyMoved = true;
  }

  if (dpState == false && yAxisValue > maxThreshold && !isYJoyMoved) {
    if (currentDisplay > 0) {
      currentDisplay--;
    } else {
      currentDisplay = 3;
    }
    isYJoyMoved = true;
  }

  if (dpState == true) {
    xAxisValue = analogRead(pinX);
    if (xAxisValue < minThreshold && !isXJoyMoved) {
      if (digitsValues[currentDisplay] > 0) {
        digitsValues[currentDisplay]--;
      } else {
        digitsValues[currentDisplay] = 9;
      }
      isXJoyMoved = true;
    }
    
    if (xAxisValue > maxThreshold && !isXJoyMoved) {
      if (digitsValues[currentDisplay] < 9) {
        digitsValues[currentDisplay]++;
      } else {
        digitsValues[currentDisplay] = 0;
      }
      isXJoyMoved = true;
    }
  }

  if (isYJoyMoved && yAxisValue >= minThreshold && yAxisValue <= maxThreshold) {
    isYJoyMoved = false;
  }

  if (isXJoyMoved && xAxisValue >= minThreshold && xAxisValue <= maxThreshold) {
    isXJoyMoved = false;
  }

  for (int i = 0; i < noOfDigits; i++)
  {
      showDigit(i);
      displayNumber(digitsValues[i],dpsStates[i]);
      delay(1);
  }
}