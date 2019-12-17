#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"
#include "Levels.h"

LedControl lc = LedControl(12, 11, 10, 1);

const int joyX = A0;
const int joyY = A1;
const int buttonPin = A2;

int valX = 0;
int buttonState = LOW;
int fireLedRow = 0;
int copyShipCol = 0;
int lastButtonState = LOW;
int fireMatrixRow = 0;
int fireMatrixCol = 0;
int nrLines = 0;
int nrClearLines = 0;
int lvlRow = 7;
int nrLives = 3;

unsigned int levelDificulty = 2500;

unsigned long movingTime = 0;
unsigned long fireDelay = 0;
unsigned long lastDebounceTimeGame = 0;
unsigned long debounceDelay = 50;
unsigned long levelLineTime = 0;
unsigned long resetLevelTime = 0;

boolean isJoyTriggered = false;
boolean levelDeployed = false;
boolean gameOver = false;
boolean levelComplete = false;
boolean hasGameStarted = false;

// ------------ variables for the lcd display ------------------

const int RSPin = 8;
const int EPin = 9;
const int D4Pin = 5;
const int D5Pin = 4;
const int D6Pin = 3;
const int D7Pin = 2;

int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;

bool joyMoved = false;
bool YjoyMoved = false;
bool isRefreshed = false;

int minThreshold = 400;
int maxThreshold = 600;
int currentPositionMenu1 = 1;
int level = 1;
int position = 0;
int lastButtonStateMenu = 0;
int buttonStateMenu = HIGH;
int flag = 0;
int flag1 = 0;
int Score = 0;
int highScore = 0;

unsigned long lastDebounceTime = 0;
unsigned long treshholdTime = 50;
unsigned long currentTime = 0;
unsigned long pressingTime = 0;
unsigned long startGameTime = 0;
unsigned long gameOverTime = 0;

const String textMenu[] = {" Start", ">Start", " High Score", ">High Score",
                           " Settings", ">Settings", "Lives:", "Level:", "Score:",
                           "HighScore:", "Starting level:"};

LiquidCrystal lcd(RSPin, EPin, D4Pin, D5Pin, D6Pin, D7Pin);

struct shipCenterPoint
{
  int row = 7;
  int currentCol = 3;
  int prevCol = -1;
} ship;

boolean isShipColliding(int row, int col)
{
  if (ship.currentCol == col && ship.row == row)
    return true;
  if (ship.currentCol + 1 == col && ship.row == row)
    return true;
  if (ship.currentCol - 1 == col && ship.row == row)
    return true;
  if (ship.currentCol + 1 == col && ship.row - 1 == row)
    return true;
  if (ship.currentCol - 1 == col && ship.row - 1 == row)
    return true;
  return false;
}

void increaseDificulty()
{
  levelDificulty -= 200;
}

void makeLinesZero(int row)
{
  for (int col = 0; col < 8; col++)
  {
    if (!isShipColliding(row, col))
    {
      gamplayMatrix[row][col] = 0;
      lc.setLed(0, row, col, gamplayMatrix[row][col]);
    }
  }
}

void showShip()
{
  lc.setLed(0, ship.row, ship.currentCol, true);
  lc.setLed(0, ship.row, ship.currentCol - 1, true);
  lc.setLed(0, ship.row, ship.currentCol + 1, true);
  lc.setLed(0, ship.row - 1, ship.currentCol - 1, true);
  lc.setLed(0, ship.row - 1, ship.currentCol + 1, true);
}

void resetMatrix()
{
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      gamplayMatrix[row][col] = 0;
    }
  }
}

void resetLevel()
{
  if (millis() - resetLevelTime >= 1000)
  {
    resetMatrix();
    levelDeployed = false;
    gameOver = false;
    levelComplete = false;
    for (int row = 0; row < 8; row++)
    {
      makeLinesZero(row);
    }
    if (!gameOver && level < 5)
    {
      level += 1;
      increaseDificulty();
    }
    showShip();
    //Serial.println("fct reset");
  }
}

void moveShip(bool position) //if position is 1 ship has moved to right else to left
{
  ship.prevCol = ship.currentCol;
  if (position)
  {
    lc.setLed(0, ship.row, ship.prevCol - 1, false);
    lc.setLed(0, ship.row - 1, ship.currentCol - 1, false);
    lc.setLed(0, ship.row - 1, ship.currentCol + 1, false);
  }
  else
  {
    lc.setLed(0, ship.row, ship.prevCol + 1, false);
    lc.setLed(0, ship.row - 1, ship.currentCol + 1, false);
    lc.setLed(0, ship.row - 1, ship.currentCol - 1, false);
  }
}

void readJoyStick()
{
  valX = analogRead(joyX);
  if (!isJoyTriggered)
  {
    movingTime = millis();
    isJoyTriggered = true;
  }

  if (valX < 300)
  {
    if (millis() - movingTime >= 200 && ship.currentCol > 1 && !levelComplete)
    {
      moveShip(false);
      ship.currentCol--;
      showShip();
      isJoyTriggered = false;
    }
  }
  else
  {
    if (valX > 700)
    {
      if (millis() - movingTime >= 200 && ship.currentCol < 6 && !levelComplete)
      {
        moveShip(true);
        ship.currentCol++;
        showShip();
        isJoyTriggered = false;
      }
    }
  }
}

void destroyBlock(int row, int col) //0 for left cannon, 1 for right cannon
{
  if (gamplayMatrix[row][col] == 1)
  {
    gamplayMatrix[row][col] = 0;
    fireMatrix[row][col] = 0;
    lc.setLed(0, row, col, false);
  }
  Score += 1;
}

boolean isBulletHittingBlock(int row, int col)
{
  if (gamplayMatrix[row][col] == 1)
    return true;
  else
    return false;
}

void fireCannon()
{
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState)
  {
    lastDebounceTimeGame = millis();
  }

  if (millis() - lastDebounceTimeGame > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == LOW)
      {
        fireMatrixRow = ship.row - 2;
        if (isBulletHittingBlock(fireMatrixRow, ship.currentCol + 1))
        {
          destroyBlock(fireMatrixRow, ship.currentCol + 1);
        }
        else
        {
          fireMatrix[fireMatrixRow][ship.currentCol + 1] = 1;
          lc.setLed(0, fireMatrixRow, ship.currentCol + 1, true);
        }

        if (isBulletHittingBlock(fireMatrixRow, ship.currentCol - 1))
        {
          destroyBlock(fireMatrixRow, ship.currentCol - 1);
        }
        else
        {
          fireMatrix[fireMatrixRow][ship.currentCol - 1] = 1;
          lc.setLed(0, fireMatrixRow, ship.currentCol - 1, true);
        }

        fireDelay = millis();
      }
    }
  }

  //this is where the moving bullets are displayed
  if (millis() - fireDelay >= 20) //cycling the fireMatrix after the delay
  {
    for (fireMatrixRow = 0; fireMatrixRow < 6; fireMatrixRow++)
    {
      for (fireMatrixCol = 0; fireMatrixCol < 8; fireMatrixCol++)
      {
        if (fireMatrix[fireMatrixRow][fireMatrixCol] == 1) //if an element of the matrix is 1 it is moved above
        {
          fireMatrix[fireMatrixRow][fireMatrixCol] = 0;
          fireMatrix[fireMatrixRow - 1][fireMatrixCol] = 1;
          if (isBulletHittingBlock(fireMatrixRow - 1, fireMatrixCol))
          {
            destroyBlock(fireMatrixRow - 1, fireMatrixCol);
          }
          else
          {
            lc.setLed(0, fireMatrixRow - 1, fireMatrixCol, true);
          }
          lc.setLed(0, fireMatrixRow, fireMatrixCol, false);
        }
      }
    }
    fireDelay = millis();
  }

  lastButtonState = reading;
}

boolean isGameplayMatrixEmpty()
{
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      if (gamplayMatrix[row][col] == 1)
        return false;
    }
  }
  return true;
}

void deployingLevel()
{
  if (!levelDeployed)
  {
    levelLineTime = millis();
    levelDeployed = true;
    nrLines = 0;
    lvlRow = 7;
    levelComplete = false;
    nrClearLines = 0;
  }

  //after moving the level matrix into gameplay matrix, the dots continue to drop until the gameplay matrix is empty
  if (millis() - levelLineTime >= levelDificulty && !gameOver && !levelComplete)
  {
    for (int row = 7; row > 0; row--)
    {
      for (int col = 0; col < 8; col++)
      {
        if (isShipColliding(row, col) && gamplayMatrix[row - 1][col] == 1)
        {
          gameOver = true;
          if (nrLives > 1)
            Score = 0;
          nrLives -= 1;
          resetLevelTime = millis();
          lc.clearDisplay(0);
          break;
        }
        else if (!isShipColliding(row, col))
        {
          gamplayMatrix[row][col] = gamplayMatrix[row - 1][col];
          lc.setLed(0, row, col, gamplayMatrix[row][col]);
        }
      }
      if (gameOver)
        break;
    }

    if (lvlRow >= 0 && !levelComplete && !gameOver)
    {
      for (int col = 0; col < 8; col++)
      {
        gamplayMatrix[0][col] = levelArray[level - 1][lvlRow][col];
        lc.setLed(0, 0, col, gamplayMatrix[0][col]);
      }
    }

    if (lvlRow < 0 && !levelComplete && !gameOver)
    {
      makeLinesZero(nrClearLines);
      nrClearLines += 1;
    }
    levelLineTime = millis();
    lvlRow--;
  }

  if (isGameplayMatrixEmpty() && lvlRow < 6 && nrLives > 0) //detects when the level is complete
  {
    levelComplete = true;
    for (int row = 0; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
        lc.setLed(0, row, col, false);
      }
    }
  }

  //necessary for the collision of the dots with the ship
  if (!gameOver)
  {
    for (int row = 6; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
        if (isShipColliding(row, col) && gamplayMatrix[row][col] == 1)
        {
          gameOver = true;
          if (nrLives > 1)
            Score = 0;
          nrLives -= 1;
          resetLevelTime = millis();
          lc.clearDisplay(0);
          break;
        }
      }
      if (gameOver)
        break;
    }
  }

  if (gameOver)
    resetLevel();
}

// ------------------- code for lcd display ---------------------

void readXAxis()
{
  int xValue = analogRead(joyX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentPositionMenu1 > 1)
    {
      currentPositionMenu1--;
    }
    else
    {
      currentPositionMenu1 = 3;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false)
  {
    if (currentPositionMenu1 < 3)
    {
      currentPositionMenu1++;
    }
    else
    {
      currentPositionMenu1 = 1;
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
  int yValue = analogRead(joyY);
  if (yValue < minThreshold && YjoyMoved == false)
  {
    if (level < 5)
    {
      level++;
    }
    else
    {
      level = 1;
    }
    YjoyMoved = true;
  }

  if (yValue > maxThreshold && YjoyMoved == false)
  {
    if (level > 1)
    {
      level--;
    }
    else
    {
      level = 5;
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

  if (reading != lastButtonStateMenu)
  {
    flag = 0;
  }

  if ((millis() - lastDebounceTime) > treshholdTime)
  {

    if (reading != buttonStateMenu)
    {
      buttonStateMenu = reading;
      if (buttonStateMenu == LOW)
      {
        flag = 1;
      }
    }
  }
  lastButtonStateMenu = reading;

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
  if (!hasGameStarted)
  {
    hasGameStarted = true;
    resetMatrix();
  }
  if (nrLives > 0 && !levelComplete)
  {
    lcd.setCursor(0, 0);
    lcd.print("Lives:");
    lcd.print(nrLives);
    lcd.setCursor(8, 0);
    lcd.print("Level:");
    lcd.setCursor(14, 0);
    lcd.print(level);
    lcd.setCursor(0, 1);
    lcd.print("Score:");
    lcd.print(Score);

    readJoyStick();
    fireCannon();
    deployingLevel();
    flag1 = false;
  }
  else if (level == 5 && levelComplete)
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
      pressingTime = millis();
    }

    lcd.setCursor(3, 0);
    lcd.print("Congrats!");
    lcd.setCursor(2, 1);
    lcd.print("Your score:");
    lcd.print(Score);
    exitMenu();
  }
  else if (levelComplete)
  {
    if (!flag1)
    {
      resetLevelTime = millis();
      flag1 = true;
    }
    resetLevel();
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
    lcd.print("Game over!");
    lcd.setCursor(2, 1);
    lcd.print("Your score:");
    lcd.print(Score);
    exitMenu();
  }
}

void principalMenu()
{
  readXAxis();
  switch (currentPositionMenu1)
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
      if (currentPositionMenu1 == 1)
      {
        position = 1;
        Score = 0;
        isRefreshed = false;
        pressingTime = millis();
        nrLives = 3;
        lcd.clear();
        showShip();
        levelComplete = false;
        levelDificulty = 2500 - (level * 150);
        hasGameStarted = false;
        levelDeployed = false;
        delay(250);
      }

      if (currentPositionMenu1 == 2)
      {
        position = 2;
        pressingTime = millis();
        lcd.clear();
      }

      if (currentPositionMenu1 == 3)
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
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 6);
  lc.clearDisplay(0);

  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      lc.setLed(0, row, col, false);
    }
  }

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
