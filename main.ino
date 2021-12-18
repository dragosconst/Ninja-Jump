#include "LedControl.h"
#include <LiquidCrystal.h>
#include "Menu.h"
#include "World.h"
#include "Player.h"
#include "RWHelper.h"
#include "StateMachine.h"

enum Difficulties {Easy = 0, Normal, Hard};

const byte dinPin = 11;
const byte clockPin = 12;
const byte loadPin = 9;

const byte xPin = A0;
const byte yPin = A1;
const byte swPin = A2; // don't have enough pins unfortunately

const byte btPin = A3;
bool btnState = HIGH;
byte btDebounce = 50;
byte btReading = HIGH;
byte previousBtReading = HIGH;
long lastBtPush = 0;
bool btPushed = LOW;
bool previousBtPush = LOW;

const byte contrastPin = 3;
const byte RSPin = 8;
const byte EPin = 13;
const byte D4 = 7;
const byte D5 = 6;
const byte D6 = 4;
const byte D7 = 2;
const byte brightnessPin = 10;

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

StateMachine sm;
int difficulty = Normal;

Player* player;
World* world;

// Menu greetingsMenu, mainMenu, settingsMenu, aboutMenu, highScoreMenu, playStats, gameOver, congrats, enterName;

// should probably dynamically alocate these too
Option* grOptsArr[1];
Option* grOptsAbArr[2];
Option* grOptsMnArr[4];
Option* grOptsStArr[5];
Option* grOptsPlArr[2];
Option* grOptsHsArr[7];
Option* grOptsGoArr[1];
Option* grOptsCgArr[2];
Option* grOptsNmArr[2];
Vector<Option*> grOpts(grOptsArr), grOptsAb(grOptsAbArr), grOptsMn(grOptsMnArr), grOptsSt(grOptsStArr), grOptsPl(grOptsPlArr), grOptsGo(grOptsGoArr),
grOptsHs(grOptsHsArr), grOptsCg(grOptsCgArr), grOptsNm(grOptsNmArr);
Menu* currentMenu;

Menu* createWelcomeMenu();
Menu* createMainMenu();
Menu* createAboutMenu();
Menu* createScoreMenu();
Menu* createSettingsMenu();
Menu* createDisplayMenu();
Menu* createNameMenu();
Menu* createCongratulationsMenu();
Menu* createGameOverMenu();

Menu* createWelcomeMenu() { 
  Menu* menu = new Menu(&grOpts, &lcd, true, 3000);
  menu->clear();
  const char* _welcomeText = "   Welcome! \n\0";
  MenuOption* _welcomeMessage = new MenuOption(_welcomeText, createMainMenu);
  grOpts.push_back(_welcomeMessage);
  return menu;
}

Menu* createMainMenu() {
  MenuOption* _playOption = new MenuOption("Play", createDisplayMenu);
  MenuOption*_settingsOption = new MenuOption("Settings\n", createSettingsMenu);
  MenuOption* _aboutOption = new MenuOption("About", createAboutMenu);
  MenuOption* _highScoreOption = new MenuOption("Scores\n", createScoreMenu);
  grOptsMn.push_back(_playOption); grOptsMn.push_back(_settingsOption); grOptsMn.push_back(_aboutOption); grOptsMn.push_back(_highScoreOption);
  Menu* menu = new Menu(&grOptsMn, &lcd, false);
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
    // creating the score text is much more tedious than it seems, but it's mostly due to memory restrictions
    // using strings would have reduced this to probably a couple lines of code
    for(byte i = 1; i <= RWHelper::readHighNum(); ++i) {
      char textVal[18];
      int index = 0;
      char name[3];
      int score = RWHelper::readHigh(i, name);
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

Menu* createNameMenu() {
  Menu* menu = new Menu(&grOptsNm, &lcd, false);
  NameOption* _enterName = new NameOption("Name:", player, RWHelper::writeHigh, createMainMenu);
  grOptsNm.push_back(_enterName);
  return menu;
}

Menu* createCongratulationsMenu() {
  Menu* menu = new Menu(&grOptsCg, &lcd, true, 2000);
  MenuOption* _congratulations = new MenuOption("Congratulations\n", createNameMenu);
  GreetingOption* _congratulationsLine2 = new GreetingOption("on new high s.\n");
  grOptsCg.push_back(_congratulations); grOptsCg.push_back(_congratulationsLine2);
  return menu;
}

Menu* createDisplayMenu() {
  Menu* menu = new Menu(&grOptsPl, &lcd, false, 0, true);
  DisplayOption* _height = new DisplayOption("Height: ", player->getHeightAddr(), true, menu);
  DisplayOption* _lives = new DisplayOption("Lives: ", player->getLivesAddr(), true, menu);
  grOptsPl.push_back(_height); grOptsPl.push_back(_lives);
  return menu;
}

Menu* createGameOverMenu() {
  MenuOption* _gameOverOption = new MenuOption("  Game over!\n", createMainMenu);
  grOptsGo.push_back(_gameOverOption);
  Menu* menu = new Menu(&grOptsGo, &lcd, true, 2000);
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

  lcd.begin(16, 2);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(btPin, INPUT_PULLUP);

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, RWHelper::readContrast());
  analogWrite(brightnessPin, RWHelper::readLCDBright());
  // RWHelper::clear();
  
  difficulty = RWHelper::readDiff();
  // player = Player(3, 10, 2, 14, &world);
  randomSeed(analogRead(0));
  // world = World(&lc, &player, difficulty);
  currentMenu = createWelcomeMenu();
  sm = StateMachine(&currentMenu, &player, &world, &lc, &difficulty);
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
  if(sm.getState() == BrowsingMenus && !newMovement(xVal, yVal))
    return;
  if(sm.getState() == BrowsingMenus) {
    xVal = refineInput(xVal);
    yVal = refineInput(yVal);
    currentMenu->joystickInput(xVal, yVal);
  }
  else {
    if(millis() - Player::lastMoved >= (!(player->isJumping() || player->isFalling()) ? Player::moveInterval : Player::moveIntervalInAir)){
      xVal = refineInput(xVal);
      yVal = refineInput(yVal);
      // if(xVal)
      //   xVal *= -1; // seems to be reversed
      player->move(xVal, yVal);
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

  if(sm.getState() == BrowsingMenus) {
    currentMenu->joystickClicked(&currentMenu);
  }
  previousSwReading = swReading;
}

void loop() {  
  sm.updateState();

  // game over
  // if(currentState == PlayingGame && player.getLives() <= 0) {
  //   currentState = BrowsingMenus;
  //   currentMenu->clear();
  //   Menu* oldMenu = currentMenu;
  //   if(RWHelper::getLastHigh() < player.getHeight()) {
  //     currentMenu = createCongratulationsMenu();
  //   }
  //   else {
  //     currentMenu = createGameOverMenu();
  //     player.clear(3, 10, 2, 14);
  //   }
  //   oldMenu->freeOptions();
  //   delete oldMenu;
  // }

  if(sm.getState() == PlayingGame) {
    btReading = digitalRead(btPin);
    if(newBtnPress() && !btPushed && player->onStableGround()) {
      btPushed = HIGH;
      player->startJumping();
    }
    // if(!newBtnPress() && btPushed && btReading == LOW && player.onStableGround()) {
    //   player.startJumping();
    // }
    else if(!newBtnPress() && btPushed && btReading == HIGH) {
      player->stopJumping();
      Player::lastFell = millis(); // pause for brief moment in the air
      btPushed = LOW;
    }
    // if(btPushed == LOW || millis() - Player::lastJumped >= Player::maxJump) {
    //   player.stopJumping();
    // }
    // else {
    // }
    previousBtReading = btReading;

    if(player->isJumping() && millis() - Player::lastJumped < Player::maxJump) {
      if(millis() - Player::lastMovedJump >= Player::jumpInterval) {
        player->jump();
        Player::lastMovedJump = millis();
      }
    }
    else if(player->isJumping()) {
      player->stopJumping();
      Player::lastFell = millis(); // pause for brief moment in the air
    }
  }

  if(sm.getState() == PlayingGame) {
    world->drawOnMatrix();
  }
  currentMenu->drawMenu();
  currentMenu->checkDisplayValues();
  currentMenu->blinkCursor();
  currentMenu->blinkUpDown();
  if(sm.getState() == PlayingGame) {
    if(millis() - Player::lastFell >= Player::fallInterval) {
      player->fall(); // check if they should fall
      Player::lastFell = millis();
    }
  }
  handleJoyInputs();
  handleJoyClick();
  if(currentMenu->isGreeting()) {
    currentMenu->killSelf(&currentMenu);
  }

}
