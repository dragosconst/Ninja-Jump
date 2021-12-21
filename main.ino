#include "LedControl.h"
#include <LiquidCrystal.h>
#include "Menu.h"
#include "World.h"
#include "Player.h"
#include "RWHelper.h"
#include "StateMachine.h"

#define WELCOME_SIZE 1  
#define ABOUT_SIZE 2
#define MAIN_SIZE 4
#define SET_SIZE 8
#define PLAY_SIZE 2
#define HS_SIZE 7
#define GO_SIZE 2
#define CG_SIZE 2
#define NAME_SIZE 2

const byte dinPin = 11;
const byte clockPin = 12;
const byte loadPin = A3;

const byte xPin = A0;
const byte yPin = A1;
const byte swPin = A2; // don't have enough pins unfortunately

const byte btPin = A5;
bool btnState = HIGH;
byte btDebounce = 50;
byte btReading = HIGH;
byte previousBtReading = HIGH;
long lastBtPush = 0;
bool btPushed = LOW;
bool previousBtPush = LOW;

const byte contrastPin = 9;
const byte RSPin = 8;
const byte EPin = 13;
const byte D4 = 7;
const byte D5 = A4;
const byte D6 = 4;
const byte D7 = 2;
const byte brightnessPin = 10;

const byte RXpin = 5; //RX for mp3 player
const byte TXpin = 6; // TX for mp3 player
const byte SFXpin = 3;
SoftwareSerial mp3Serial(TXpin, RXpin);
DFRobotDFPlayerMini DFPplayer;

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
int difficulty = 1;

Player* player;
World* world;

Menu* currentMenu;

int currentScore;
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
  Menu* menu = new Menu(&lcd, true, 3000);
  menu->clear();
  const char* _welcomeText = "   Welcome! \n\0";
  Option** grOptsArr = new Option*[WELCOME_SIZE];
  MenuOption* _welcomeMessage = new MenuOption(_welcomeText, createMainMenu);
  grOptsArr[0] = _welcomeMessage;
  menu->setOptions(grOptsArr, WELCOME_SIZE);
  return menu;
}

Menu* createMainMenu() {
  Option** grOptsMnArr = new Option*[MAIN_SIZE];
  MenuOption* _playOption = new MenuOption("Play", createDisplayMenu);
  MenuOption*_settingsOption = new MenuOption("Settings\n", createSettingsMenu);
  MenuOption* _aboutOption = new MenuOption("About", createAboutMenu);
  MenuOption* _highScoreOption = new MenuOption("Scores\n", createScoreMenu);
  grOptsMnArr[0] = _playOption; grOptsMnArr[1] = _settingsOption; grOptsMnArr[2] = _aboutOption; grOptsMnArr[3] = _highScoreOption;
  Menu* menu = new Menu(&lcd, false);
  menu->setOptions(grOptsMnArr, MAIN_SIZE);
  return menu;
}

Menu* createAboutMenu() {
  Option** grOptsAbArr = new Option*[ABOUT_SIZE];
  MenuOption* _aboutLineOne = new MenuOption("Check my stuff\n", createMainMenu);
  MenuOption* _aboutLineTwo = new MenuOption("bit.ly/3EBftyx\n", createMainMenu);
  grOptsAbArr[0] = _aboutLineOne; grOptsAbArr[1] = _aboutLineTwo;
  Menu* menu = new Menu(&lcd, false);
  menu->setOptions(grOptsAbArr, ABOUT_SIZE);
  return menu;
}

Menu* createSettingsMenu() {
  Option** grOptsStArr = new Option*[SET_SIZE];
  SystemOption* _contrastOption = new SystemOption("Contrast", contrastPin, baseContrast, RWHelper::getVal(CONTRAST_ADDR), 10, RWHelper::writeContrast);
  SystemOption* _brightOption = new SystemOption("LCD brg.", brightnessPin, baseLCDBrightness, RWHelper::getVal(LCD_ADDR), 20, RWHelper::writeLCDBright);
  LEDOption* _ledOption = new LEDOption("LED brg.", &lc, RWHelper::getVal(LED_ADDR));
  GameOption* _diffOption = new GameOption("Diff.", &difficulty, 1, 2, RWHelper::getVal(DIFF_ADDR), gameValue);
  GameOption* _volOption = new GameOption("Volume", nullptr, 0, 5, RWHelper::getVal(RWHelper::volAddr), volumeOption);
  GameOption* _themeOption = new GameOption("Game theme", nullptr, 1, 2, RWHelper::getVal(RWHelper::themeAddr), themeOption);
  GameOption* _soundOption = new GameOption("Sounds", nullptr, 0, 1, RWHelper::getVal(RWHelper::soundAddr), soundOption);
  MenuOption* _backSetOption = new MenuOption("Back\n", createMainMenu);
  grOptsStArr[0]  = _contrastOption; grOptsStArr[1]  = _brightOption; grOptsStArr[2]  = _ledOption; grOptsStArr[3]  = _diffOption; grOptsStArr[4]  = _volOption; grOptsStArr[5]  = _themeOption; grOptsStArr[6]  = _soundOption; grOptsStArr[7]  = _backSetOption;
  Menu* menu = new Menu(&lcd, false);
  menu->setOptions(grOptsStArr, SET_SIZE);
  return menu;
}

Menu* createScoreMenu() {
  Menu* menu = new Menu(&lcd, false);
  Option** grOptsHsArr = new Option*[HS_SIZE];
  MenuOption* _backFromScore = new MenuOption("Back\n", createMainMenu);
  if(RWHelper::getVal(HSNUM_ADDR) == 0) {
    GreetingOption* _noScores = new GreetingOption("No scores...\n");
    grOptsHsArr[0] = _noScores;
    grOptsHsArr[1] = _backFromScore;
    menu->setOptions(grOptsHsArr, 2);
  }
  else {
    // creating the score text is much more tedious than it seems, but it's mostly due to memory restrictions
    // using strings would have reduced this to probably a couple lines of code
    for(byte i = 1; i <= RWHelper::getVal(HSNUM_ADDR); ++i) {
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
      grOptsHsArr[i - 1]  = _score;
    }
    grOptsHsArr[RWHelper::getVal(HSNUM_ADDR)] = _backFromScore;
    menu->setOptions(grOptsHsArr, RWHelper::getVal(HSNUM_ADDR) + 1);
  }
  return menu;
}

GreetingOption* createRankOption() {
  GreetingOption* _rankOption;
  if(currentScore <= RONIN) {
    _rankOption = new GreetingOption("Rank: Ronin\n");
  }
  else if(currentScore <= APPRENTICE) {
    _rankOption = new GreetingOption("Rank: Fledgling\n");
  }
  else if(currentScore <= NINJA) {
    _rankOption = new GreetingOption("Rank: Ninja\n");
  }
  else if(currentScore <= SAMURAI) {
    _rankOption = new GreetingOption("Rank: Samurai\n");
  }
  else if(currentScore <= ASSASSIN) {
    _rankOption = new GreetingOption("Rank: Assassin\n");
  }
  else if(currentScore <= MONK) {
    _rankOption = new GreetingOption("Rank: Monk\n");
  }
  else if(currentScore <= SHOGUN) {
    _rankOption = new GreetingOption("Rank: Shogun\n");
  }
  else {
    _rankOption = new GreetingOption("Rank: Sekiro\n");
  }
  return _rankOption;
}

Menu* createNameMenu() {
  Option** grOptsNmArr = new Option*[NAME_SIZE];
  Menu* menu = new Menu(&lcd, false);
  NameOption* _enterName = new NameOption("Name:", currentScore, createMainMenu);
  GreetingOption* _rankOption = createRankOption();
  grOptsNmArr[0] = _enterName; grOptsNmArr[1] = _rankOption;
  menu->setOptions(grOptsNmArr, NAME_SIZE);
  return menu;
}

Menu* createCongratulationsMenu() {
  Option** grOptsCgArr = new Option*[CG_SIZE];
  currentScore = player->getHeight();
  Menu* menu = new Menu(&lcd, true, 2000);
  MenuOption* _congratulations = new MenuOption("Congratulations\n", createNameMenu);
  GreetingOption* _congratulationsLine2 = new GreetingOption("on new high s.\n");
  grOptsCgArr[0] = _congratulations; grOptsCgArr[1] = _congratulationsLine2;
  SoundsManager::switchMenuState(false);
  menu->setOptions(grOptsCgArr, CG_SIZE);
  return menu;
}

Menu* createDisplayMenu() {
  Option** grOptsPlArr = new Option*[PLAY_SIZE];
  Menu* menu = new Menu(&lcd, false, 0, true);
  DisplayOption* _height = new DisplayOption("Height: ", player->getHeightAddr(), menu);
  DisplayOption* _lives = new DisplayOption("Lives: ", player->getLivesAddr(), menu);
  grOptsPlArr[0]  =_height; grOptsPlArr[1] = _lives;
  SoundsManager::switchMenuState(false);
  menu->setOptions(grOptsPlArr, PLAY_SIZE);
  return menu;
}

Menu* createGameOverMenu() {
  Option** grOptsGoArr = new Option*[GO_SIZE];
  currentScore = player->getHeight();
  MenuOption* _gameOverOption = new MenuOption("  Game over!\n", createMainMenu);
  GreetingOption* _rankOption = createRankOption();
  grOptsGoArr[0]  = _gameOverOption; grOptsGoArr[1]  = _rankOption;
  Menu* menu = new Menu(&lcd, true, 2000);
  SoundsManager::switchMenuState(true);
  menu->setOptions(grOptsGoArr, GO_SIZE);
  return menu;
}

void setup() {
 Serial.begin(115200);
 mp3Serial.begin(9600);

  if(!DFPplayer.begin(mp3Serial)) {
    Serial.println("s a busit");
  }
  SoundsManager::setPlayer(&DFPplayer);
  SoundsManager::changeVolume(VOL_0 + RWHelper::getVal(RWHelper::volAddr));
  SoundsManager::changeInGame(RWHelper::getVal(RWHelper::themeAddr) + 1);
  SoundsManager::playTheme();
  SoundsManager::setSounds(RWHelper::getVal(RWHelper::soundAddr));


  // tone(SFXpin, 300);
  
  lc.shutdown(0, false);
  lc.setIntensity(0, RWHelper::getVal(LED_ADDR));
  lc.clearDisplay(0);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(btPin, INPUT_PULLUP);

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, RWHelper::getVal(CONTRAST_ADDR));
  analogWrite(brightnessPin, RWHelper::getVal(LCD_ADDR));
  
  lcd.begin(16, 2);
  // lcd.print("heelloo");
  // delay(5000);
  // RWHelper::clear();
  
  difficulty = RWHelper::getVal(DIFF_ADDR);
  randomSeed(analogRead(0)); // maybe seed on each play, to add millis?
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

  if(sm.getState() == PlayingGame) {
    btReading = digitalRead(btPin);
    if(newBtnPress() && !btPushed && player->onStableGround()) {
      btPushed = HIGH;
      player->startJumping();
    }
    else if(!newBtnPress() && btPushed && btReading == HIGH) {
      player->stopJumping();
      Player::lastFell = millis(); // pause for brief moment in the air
      btPushed = LOW;
    }
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
    world->freeStructures();
    world->activateStructures();
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
