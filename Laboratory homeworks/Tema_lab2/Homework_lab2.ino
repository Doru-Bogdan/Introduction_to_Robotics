const int redLED = 11;
const int greenLED = 10;
const int blueLED = 9;

const int potRedPin = A0;
const int potGreenPin = A1;
const int potBluePin = A2;

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

void loop() {
  int potRedValue = analogRead(potRedPin);
  int potGreenValue = analogRead(potGreenPin);
  int potBlueValue = analogRead(potBluePin);

  int intensityRedLED;
  int intensityGreenLED;
  int intensityBlueLED;

  intensityRedLED = map(potRedValue, 0, 1023, 0, 255);
  intensityGreenLED = map(potGreenValue, 0, 1023, 0, 255);
  intensityBlueLED = map(potBlueValue, 0, 1023, 0, 255);
  
  analogWrite(redLED, intensityRedLED);
  analogWrite(greenLED, intensityGreenLED);
  analogWrite(blueLED, intensityBlueLED);
}
