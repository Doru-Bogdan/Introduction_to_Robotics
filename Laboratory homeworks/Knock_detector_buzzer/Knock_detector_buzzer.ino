const int buzzerInputPin = A0;
const int buzzerOutputPin = 9;
const int buttonPin = 2;

const int buzzerInputThreshold = 5;
unsigned long knockTimeRead = 0;
const unsigned long thresholdTime = 5000;
bool isBuzzerOutputOn = false;

void setup() {
  pinMode(buzzerInputPin, INPUT);
  pinMode(buzzerOutputPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int valueBuzzerInput = analogRead(buzzerInputPin);

  if (valueBuzzerInput > buzzerInputThreshold && !isBuzzerOutputOn) {
    knockTimeRead = millis();
    Serial.println("Knock detected!");
    isBuzzerOutputOn = true;
  }

  if (isBuzzerOutputOn && millis() - knockTimeRead >= thresholdTime && knockTimeRead != 0) {
    digitalWrite(buzzerOutputPin, HIGH);
    int buttonValue = !digitalRead(buttonPin);
    if (buttonValue == 1) {
      digitalWrite(buzzerOutputPin, LOW);
      isBuzzerOutputOn = false;
    }
  }
}
