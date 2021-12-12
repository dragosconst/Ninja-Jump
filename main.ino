#include "LedControl.h"
#include <LiquidCrystal.h>
#include "Menu.h"
#include "World.h"
#include "Player.h"
#include "RWHelper.h"

enum GameStates {BrowsingMenus, PlayingGame};
enum Difficulties {Easy = 0, Normal, Hard};

const int dinPin = 11;
const int clockPin = 12;
const int loadPin = 9;

const int xPin = A0;
const int yPin = A1;
const int swPin = A2; // don't have enough pins unfortunately

const int btPin = A3;
bool btnState = HIGH;
byte btDebounce = 50;
byte btReading = HIGH;
byte previousBtReading = HIGH;
long lastBtPush = 0;
bool btPushed = LOW;
bool previousBtPush = LOW;

const int contrastPin = 3;
const int RSPin = 8;
const int EPin = 13;
const int D4 = 7;
const int D5 = 6;
const int D6 = 4;
const int D7 = 2;
const int brightnessPin = 10;

byte baseContrast = 60;
byte baseLCDBrightness = 150;

const byte matrixSize = 8;

const int minThreshold = 200;
const int maxThreshold = 600;
bool joyMoved = false; // to be used for menu browsing only
bool swState = LOW;
bool clicked = LOW;
byte swDebounce = 50;
byte swReading = HIGH;
byte previousSwReading = HIGH;
long lastSwPush = 0;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;


LiquidCrystal lcd(RSPin, EPin, D4, D5, D6, D7);

GameStates currentState;
int difficulty = Normal;

Player player;
World world;

// Menu greetingsMenu, mainMenu, settingsMenu, aboutMenu, highScoreMenu, playStats, gameOver, congrats, enterName;
// MenuOption welcomeMessage;

// MenuOption playOption, settingsOption, aboutOption, highSchoreOption;

// MenuOption aboutLineOne;
// GreetingOption aboutLineTwo;

// SystemOption contrastOption, brightOption;
// LEDOption ledOption;
// GameOption diffOption; // maybe volume too?
// MenuOption backSetOption; 

// GreetingOption scores[5], noScores;
// MenuOption backFromScore;

// DisplayOption height, lives;

// MenuOption gameOverOption;

Option* grOptsArr[1];
Option* grOptsAbArr[2];
Option* grOptsMnArr[4];
Option* grOptsStArr[5];
Option* grOptsPlArr[2];
Option* grOptsHsArr[7];
Option* grOptsGoArr[1];
Vector<Option*> grOpts(grOptsArr), grOptsAb(grOptsAbArr), grOptsMn(grOptsMnArr), grOptsSt(grOptsStArr), grOptsPl(grOptsPlArr), grOptsGo(grOptsGoArr),
grOptsHs(grOptsHsArr);
Menu* currentMenu;

inline Menu* createWelcomeMenu();
Menu* createMainMenu();
Menu* createAboutMenu();
Menu* createScoreMenu();
Menu* createSettingsMenu();
Menu* createDisplayMenu();
Menu* createGameOverMenu();

inline Menu* createWelcomeMenu() { 
//  volatile int* myint = (int*)malloc(4* sizeof(int));
    // volatile int* arr3 = (int*)malloc(21 * sizeof(char));
  // volatile int* arr = (int*)malloc(2*sizeof(int));
  // volatile int* arrf = (int*)malloc(2*sizeof(float));
  Menu* menu = new Menu(&grOpts, &lcd, true, 3000);
  menu->clear();
//  Serial.println((int)arr2);
//  Serial.println(NULL);
// // Serial.println((int)arr3);
//  Serial.println((int)arr);
//  Serial.println(sizeof(Menu));
//  Serial.println((int)arrf);
//  Serial.println(sizeof(MenuOption));
//  Serial.println("stuff 3");
  const char* _welcomeText = "   Welcome! \n\0";
  MenuOption* _welcomeMessage = new MenuOption(_welcomeText, createMainMenu);
  // int* sdasda = (int*)malloc(sizeof(int));
  grOpts.push_back(_welcomeMessage);
  // Menu* menu = (Menu*)malloc(sizeof(Menu));
  // Serial.println("hello 5");
  // *menu = Menu(&grOpts, &lcd, true, 3000);
 return menu;
}

Menu* createMainMenu() {
  MenuOption* _playOption = new MenuOption("Play", createDisplayMenu);
  MenuOption*_settingsOption = new MenuOption("Settings\n", createSettingsMenu);
  MenuOption* _aboutOption = new MenuOption("About", createAboutMenu);
  MenuOption* _highScoreOption = new MenuOption("Scores\n", createScoreMenu);
  grOptsMn.push_back(_playOption); grOptsMn.push_back(_settingsOption); grOptsMn.push_back(_aboutOption); grOptsMn.push_back(_highScoreOption);
  Menu* menu = new Menu(&grOptsMn, &lcd, false);
  // Serial.println("----created main menu-----");
  // delay(500);
  return menu;
}

Menu* createAboutMenu() {
  MenuOption* _aboutLineOne = new MenuOption("Check my stuff\n", createMainMenu);
  MenuOption* _aboutLineTwo = new MenuOption("bit.ly/3EBftyx\n", createMainMenu);
  grOptsAb.push_back(_aboutLineOne); grOptsAb.push_back(_aboutLineTwo);
  Menu* menu = new Menu(&grOptsAb, &lcd, false);
  return menu;
}

Menu* createSettingsMenu() {
  SystemOption* _contrastOption = new SystemOption("Contrast", contrastPin, baseContrast, RWHelper::readContrast(), 10, true, RWHelper::writeContrast);
  SystemOption* _brightOption = new SystemOption("LCD brg.", brightnessPin, baseLCDBrightness, RWHelper::readLCDBright(), 20, true, RWHelper::writeLCDBright);
  LEDOption* _ledOption = new LEDOption("LED brg.", &lc, RWHelper::readLEDBright(), true, RWHelper::writeLEDBright);
  GameOption* _diffOption = new GameOption("Difficulty", &difficulty, difficulty, RWHelper::readDiff(), 1, 3, true, RWHelper::writeDiff);
  MenuOption* _backSetOption = new MenuOption("Back\n", createMainMenu);
  grOptsSt.push_back(_contrastOption); grOptsSt.push_back(_brightOption); grOptsSt.push_back(_ledOption); grOptsSt.push_back(_diffOption); grOptsSt.push_back(_backSetOption);
  Menu* menu = new Menu(&grOptsSt, &lcd, false);
  return menu;
}

Menu* createScoreMenu() {
  MenuOption* _backFromScore = new MenuOption("Back\n", createMainMenu);
  if(RWHelper::readHighNum() == 0) {
    GreetingOption* _noScores = new GreetingOption("No scores...\n");
    grOptsHs.push_back(_noScores);
  }
  else {
    for(byte i = 1; i <= RWHelper::readHighNum(); ++i) {
      char textVal[18];
      int index = 0;
      char name[3];
      int score;
      RWHelper::readHigh(i, name);
      char number[20];
      itoa(i, number, 10);
      for(byte j = 0; number[j]; ++j) {
        textVal[index++] = number[j];
      }
      textVal[index++] = '.';
      for(byte j = 0; j < 3; ++j) {
        textVal[index++] = name[j];
      }
      textVal[index++] = ' ';
      itoa(score, number, 10);
      for(byte j = 0; number[j]; ++j) {
        textVal[index++] = number[j];
      }
      textVal[index++] = '\n';
      textVal[index] = '\0';
      GreetingOption* _score = new GreetingOption(textVal);
      grOptsHs.push_back(_score);
    }
  }
  grOptsHs.push_back(_backFromScore);
  Menu* menu = new Menu(&grOptsHs, &lcd, false);
  return menu;
}

Menu* createDisplayMenu() {
  Menu* menu = new Menu(&grOptsPl, &lcd, false);
  DisplayOption* _height = new DisplayOption("Height: ", player.getHeightAddr(), true, menu);
  DisplayOption* _lives = new DisplayOption("Lives: ", player.getLivesAddr(), false, menu);
  grOptsPl.push_back(_height); grOptsPl.push_back(_lives);
  return menu;
}

Menu* createGameOverMenu() {
  MenuOption* _gameOverOption = new MenuOption("  Game over!\n", createMainMenu);
  grOptsGo.push_back(_gameOverOption);
  Menu* menu = new Menu(&grOptsGo, &lcd, false);
  return menu;
}

// Custom character seem to slow down the LCD by a LOT.
// byte upArrow[8] = {
//     B00000,
//     B00100,
//     B01110,
//     B10101,
//     B00100,
//     B00100,
//     B00100,
//     B00100
// };
// byte downArrow[8] = {
//         B00000,
//         B00100,
//         B00100,
//         B00100,
//         B00100,
//         B10101,
//         B01110,
//         B00100
//     };
// void createSpecialChars(LiquidCrystal* lcd) {
//   lcd->createChar(UP_ARROW, upArrow);
//   lcd->createChar(DOWN_ARROW, downArrow);
// }

void setup() {
  Serial.begin(9600);
  
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, RWHelper::readLEDBright()); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  lc.setRow(0, 1, B11111111);

  lcd.begin(16, 2);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(btPin, INPUT_PULLUP);

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, RWHelper::readContrast());
  analogWrite(brightnessPin, RWHelper::readLCDBright());
  Serial.print("brightness is");
  Serial.println(RWHelper::readLCDBright());

//   RWHelper::clear();
  
  difficulty = RWHelper::readDiff();
  player = Player(3, 10, 2, 14, &world);
  world = World(&lc, &player);
  currentMenu = createWelcomeMenu();
  currentState = BrowsingMenus;
  // Serial.println("restarted-------------------------    ");
  // delay(1000);
}

// refine inputs to be used with menus, we don't really care to know its exact value, only whether it's activated positively or not
// i'll probably write this somewhere else, too, but 1 on x means right (and -1 left), and 1 on y means down (and -1 up)
int refineInput(int input) {
  if(input > maxThreshold) 
    return 1;
  else if(input < minThreshold)
    return -1;
  return 0;
}

bool newMovement(int xVal, int yVal) {
  if((xVal >= minThreshold && xVal <= maxThreshold) && (yVal >= minThreshold && yVal <= maxThreshold) && joyMoved) {
    joyMoved = false;
    return false;
  }

  if((xVal > maxThreshold || yVal > maxThreshold || xVal < minThreshold || yVal < minThreshold) && !joyMoved) {
    joyMoved = true;
    return true;
  }
  return false;
}

bool newJoyClick() {
  if(swReading != previousSwReading) {
    lastSwPush = millis();
//    Serial.println("heee hei");
  }

  if(millis() - lastSwPush > swDebounce) {
    swState = swReading;
    if(swState == LOW)
      return HIGH;
  }
  return LOW;
}

bool newBtnPress() {
  if(btReading != previousBtReading) {
    lastBtPush = millis();
  }

  if(millis() - lastBtPush > btDebounce) {
    btnState = btReading;
    if(btnState == LOW)
      return HIGH;
  }
  return LOW;
}

void handleJoyInputs() {
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  if(currentState == BrowsingMenus && !newMovement(xVal, yVal))
    return;
  if(currentState == BrowsingMenus) {
    xVal = refineInput(xVal);
    yVal = refineInput(yVal);
    currentMenu->joystickInput(xVal, yVal);
  }
  else {
    if(millis() - Player::lastMoved >= (player.onStableGround() ? Player::moveInterval : Player::moveIntervalInAir)){
      xVal = refineInput(xVal);
      yVal = refineInput(yVal);
      if(xVal)
        xVal *= -1; // seems to be reversed
      player.move(xVal, yVal);
      Player::lastMoved = millis();
    }
  }
}

void handleJoyClick() {
  swReading = digitalRead(swPin);
  bool clickResult = newJoyClick();
  if(clickResult && clicked == HIGH) {
    previousSwReading = swReading;
    return;
  }
  if(!clickResult) {
    clicked = LOW;
    previousSwReading = swReading;
    return;
  }

  clicked = HIGH;

  if(currentState == BrowsingMenus) {
    currentMenu->joystickClicked(&currentMenu);
  }
  previousSwReading = swReading;
}

void loop() {  
  // if(currentMenu == &playStats && currentState == BrowsingMenus) {
  //   currentState = PlayingGame;
  // }
  // else if(currentMenu != &playStats && currentState == PlayingGame) {
  //   currentState = BrowsingMenus;
  // } 

  // if(currentState == PlayingGame && player.getLives() <= 0) {
  //   currentState = BrowsingMenus;
  //   currentMenu->clear();
  //   player.clear(3, 10, 2, 14);
  //   currentMenu = &gameOver;
  // }

  if(currentState == PlayingGame) {
    btReading = digitalRead(btPin);
    if(newBtnPress() && !btPushed && player.onStableGround()) {
      btPushed = HIGH;
      player.startJumping();
    }
    // if(!newBtnPress() && btPushed && btReading == LOW && player.onStableGround()) {
    //   player.startJumping();
    // }
    else if(!newBtnPress() && btPushed && btReading == HIGH) {
      player.stopJumping();
      btPushed = LOW;
    }
    if(btPushed == LOW || millis() - Player::lastJumped >= Player::maxJump) {
      player.stopJumping();
    }
    else {
    }
    previousBtReading = btReading;

    if(player.isJumping() && millis() - Player::lastJumped < Player::maxJump) {
      if(millis() - Player::lastMovedJump >= Player::jumpInterval) {
        player.jump();
        Player::lastMovedJump = millis();
      }
    }
    else {
      player.stopJumping();
    }
  }

  world.drawOnMatrix();
  // Serial.println("before drawing");
  // delay(500);
  currentMenu->drawMenu();
  // Serial.println("drawing");
  // delay(500);
  currentMenu->printValues();
  currentMenu->checkDisplayValues();
  // Serial.println("checking");
  // delay(500);
  currentMenu->blinkCursor();
  // Serial.println("blinking");
  // delay(500);
  currentMenu->blinkUpDown();
  // Serial.println("updown");
  // delay(500);
  if(currentState == PlayingGame) {
    if(millis() - Player::lastFell >= Player::fallInterval) {
      player.fall(); // check if they should fall
      Player::lastFell = millis();
    }
  }
  handleJoyInputs();
  // Serial.println("joying");
  // delay(500);
  handleJoyClick();
  // Serial.println("clicking");
  // delay(500);
  if(currentMenu->isGreeting()) {
    currentMenu->killSelf(&currentMenu);
    // Serial.println("killing");
    // delay(500);
  }

//   Serial.println(hello->getOptions());
}
