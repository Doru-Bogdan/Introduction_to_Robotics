#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int RSPin = 12;
const int EPin = 11;
const int D4Pin = 5;
const int D5Pin = 4;
const int D6Pin = 3;
const int D7Pin = 2;

const int xAxisPin = A0;
const int yAxisPin = A1;
const int buttonPin = 7;

int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;


bool joyMoved = false;
bool YjoyMoved = false;
bool isRefreshed = false;

int minThreshold = 400;
int maxThreshold = 600;
int CurrentPositionMenu1 = 1;
int level = 1;
int position = 0;
int lastButtonState = 0;
int buttonState = HIGH;
int flag = 0;
int flag1 = 0;
int Score = 0;
int highScore = 0;

unsigned long lastDebounceTime = 0;
unsigned long TreshholdTime = 50;
unsigned long currentTime = 0;
unsigned long pressingTime = 0;
unsigned long startGameTime = 0;


const String textMenu[] = {" Start", ">Start", " High Score", ">High Score",
                           " Settings", ">Settings", "Lives:", "Level:", "Score:",
                           "HighScore:", "Starting level:"};

LiquidCrystal lcd(RSPin, EPin, D4Pin, D5Pin, D6Pin, D7Pin);

void readXAxis()
{
    int xValue = analogRead(xAxisPin);
    if (xValue < minThreshold && joyMoved == false)
    {
        if (CurrentPositionMenu1 > 1)
        {
            CurrentPositionMenu1--;
        }
        else
        {
            CurrentPositionMenu1 = 3;
        }
        joyMoved = true;
    }

    if (xValue > maxThreshold && joyMoved == false)
    {
        if (CurrentPositionMenu1 < 3)
        {
            CurrentPositionMenu1++;
        }
        else
        {
            CurrentPositionMenu1 = 1;
        }
        joyMoved = true;
    }

    if (xValue >= minThreshold && xValue <= maxThreshold)
    {
        joyMoved = false;
    }
}

void changeLevel()
{
    int yValue = analogRead(yAxisPin);
    if (yValue < minThreshold && YjoyMoved == false)
    {
        if (level > 1)
        {
            level--;
        }
        else
        {
            level = 3;
        }
        YjoyMoved = true;
    }

    if (yValue > maxThreshold && YjoyMoved == false)
    {
        if (level < 3)
        {
            level++;
        }
        else
        {
            level = 1;
        }
        YjoyMoved = true;
    }

    if (yValue >= minThreshold && yValue <= maxThreshold)
    {
        YjoyMoved = false;
    }
}

int buttonPressed()
{
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState)
    {
        flag = 0;
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > TreshholdTime)
    {

        if (reading != buttonState)
        {
            buttonState = reading;
            if (buttonState == LOW)
            {
                flag = 1;
            }
        }
    }
    lastButtonState = reading;

    if (flag == 1)
    {
        return 1;
    }
    else
        return 0;
}

void exitMenu()
{
    if (millis() - pressingTime >= 250 && buttonPressed() == 1)
    {
        position = 0;
        pressingTime = millis();
        lcd.clear();
    }
}

void settingsMenu()
{
    lcd.setCursor(0, 0);
    lcd.print("Level:");
    lcd.print(level);
    changeLevel();
    exitMenu();
}

void highScoreMenu()
{
    lcd.setCursor(1, 0);
    lcd.print("High score:");
    lcd.print(highScore);
    exitMenu();
}

void startMenu()
{
    if (millis() - startGameTime <= 11000)
    {
        lcd.setCursor(0, 0);
        lcd.print("Lives:3");
        lcd.setCursor(8, 0);
        lcd.print("Level:");
        lcd.setCursor(14, 0);
        lcd.print(level);

        if (flag1 == 0)
        {
            currentTime = millis();
            flag1 = 1;
        }

        if (millis() - currentTime >= 5000)
        {
            Score = Score + 3 * level;
            if (level < 3)
            {
                level += 1;
            }
            flag1 = 0;
        }

        lcd.setCursor(0, 1);
        lcd.print("Score:");
        lcd.print(Score);
    }
    else
    {
        if (highScore < Score)
        {
            highScore = Score;
            EEPROM.write(0, highScore);
        }

        if (!isRefreshed)
        {
            lcd.clear();
            isRefreshed = true;
        }

        lcd.setCursor(3, 0);
        lcd.print("Congrats!");
        lcd.setCursor(3, 1);
        lcd.print("Press JOY");
        exitMenu();
    }
}

void principalMenu()
{
    readXAxis();
    switch (CurrentPositionMenu1)
    {
    case 1: //Start game
        lcd.setCursor(0, 0);
        lcd.print(textMenu[1]);
        lcd.setCursor(7, 0);
        lcd.print(textMenu[4]);
        lcd.setCursor(2, 1);
        lcd.print(textMenu[2]);
        break;

    case 2: //Settings
        lcd.setCursor(0, 0);
        lcd.print(textMenu[0]);
        lcd.setCursor(7, 0);
        lcd.print(textMenu[5]);
        lcd.setCursor(2, 1);
        lcd.print(textMenu[2]);
        break;

    case 3: //High score
        lcd.setCursor(0, 0);
        lcd.print(textMenu[0]);
        lcd.setCursor(7, 0);
        lcd.print(textMenu[4]);
        lcd.setCursor(2, 1);
        lcd.print(textMenu[3]);
        break;
    }

    if (millis() - pressingTime >= 250 || pressingTime == 0)
    {
        if (buttonPressed() == 1)
        {
            if (CurrentPositionMenu1 == 1)
            {
                position = 1;
                Score = 0;
                flag1 = 0;
                isRefreshed = false;
                pressingTime = millis();
                startGameTime = millis();
                lcd.clear();
            }

            if (CurrentPositionMenu1 == 2)
            {
                position = 2;
                pressingTime = millis();
                lcd.clear();
            }

            if (CurrentPositionMenu1 == 3)
            {
                position = 3;
                pressingTime = millis();
                lcd.clear();
            }
        }
    }
}

void setup()
{
    pinMode(buttonPin, INPUT_PULLUP);
    lcd.begin(16, 2);
    lcd.clear();
    highScore = EEPROM.read(0);
}

void loop()
{
    switch (position)
    {
    case 1:
        startMenu();
        break;

    case 2:
        settingsMenu();
        break;

    case 3:
        highScoreMenu();
        break;

    default:
        principalMenu();
        break;
    }
}