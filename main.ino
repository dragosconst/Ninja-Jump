#include "LedControl.h"
#include <LiquidCrystal.h>
#include "Menu.h"
#include "World.h"
#include "Player.h"

enum GameStates {BrowsingMenus, PlayingGame};
enum Difficulties {Easy = 0, Normal, Hard};

const int dinPin = 12;
const int clockPin = 13;
const int loadPin = 11;

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
const int EPin = 9;
const int D4 = 7;
const int D5 = 6;
const int D6 = 4;
const int D7 = 2;
const int brightnessPin = 10;

int contrast = 60;
int brightness = 150;

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

Menu greetingsMenu, mainMenu, settingsMenu, aboutMenu, highScoreMenu, playStats, gameOver;
MenuOption welcomeMessage;

MenuOption playOption, settingsOption, aboutOption, highSchoreOption;

MenuOption aboutLineOne;
GreetingOption aboutLineTwo;

SystemOption contrastOption, brightOption;
GameOption diffOption; // maybe volume too?
MenuOption backSetOption;

DisplayOption height, lives;

MenuOption gameOverOption;

Option* grOptsArr[1];
Option* grOptsAbArr[2];
Option* grOptsMnArr[4];
Option* grOptsStArr[4];
Option* grOptsPlArr[2];
Option* grOptsGoArr[1];
Vector<Option*> grOpts(grOptsArr), grOptsAb(grOptsAbArr), grOptsMn(grOptsMnArr), grOptsSt(grOptsStArr), grOptsPl(grOptsPlArr), grOptsGo(grOptsGoArr);
Menu* currentMenu;

void createMenus() {
  const char* _welcomeText = "   Welcome! \n\0";
  MenuOption _welcomeMessage(_welcomeText, &mainMenu);
  welcomeMessage = _welcomeMessage;
  grOpts.push_back(&welcomeMessage);
  Menu menu(&grOpts, &lcd, true, 3000);
  greetingsMenu = menu;

  MenuOption _aboutLineOne("Check my stuff\n", &mainMenu);
  GreetingOption _aboutLineTwo("bit.ly/3EBftyx\n");
  aboutLineOne = _aboutLineOne;
  aboutLineTwo = _aboutLineTwo;
  grOptsAb.push_back(&aboutLineOne); grOptsAb.push_back(&aboutLineTwo);
  Menu _aboutMenu(&grOptsAb, &lcd, false);
  aboutMenu = _aboutMenu;

  SystemOption _contrastOption("Con.", contrastPin, contrast, 10, false), _brightOption("Brg.", brightnessPin, brightness, 20, true);
  GameOption _diffOption("Diff.", &difficulty, difficulty, 1, 3, false);
  MenuOption _backSetOption("Back\n", &mainMenu);
  contrastOption = _contrastOption;
  brightOption = _brightOption;
  diffOption = _diffOption;
  backSetOption = _backSetOption;
  grOptsSt.push_back(&contrastOption); grOptsSt.push_back(&brightOption); grOptsSt.push_back(&diffOption); grOptsSt.push_back(&backSetOption);
  Menu _settingsMenu(&grOptsSt, &lcd, false);
  settingsMenu = _settingsMenu;

  DisplayOption _height("Height: ", player.getHeightAddr(), true, &playStats), _lives("Lives: ", player.getLivesAddr(), false, &playStats);
  height = _height;
  lives = _lives;
  Serial.println("im here boss");
  Serial.println((int)player.getHeightAddr());
  Serial.println(*player.getHeightAddr());
  Serial.println((int)player.getLivesAddr());
  Serial.println(*player.getLivesAddr());
  grOptsPl.push_back(&height); grOptsPl.push_back(&lives);
  Menu _playStats(&grOptsPl, &lcd, false);
  playStats = _playStats;


  MenuOption _playOption("Play", &playStats), _settingsOption("Settings\n", &settingsMenu), _aboutOption("About", &aboutMenu), _highScoreOption("Scores\n", &highScoreMenu);
  playOption = _playOption;
  settingsOption = _settingsOption;
  aboutOption = _aboutOption;
  highSchoreOption = _highScoreOption;
  grOptsMn.push_back(&playOption); grOptsMn.push_back(&settingsOption); grOptsMn.push_back(&aboutOption); grOptsMn.push_back(&highSchoreOption);
  Menu _mainMenu(&grOptsMn, &lcd, false);
  mainMenu = _mainMenu;


  MenuOption _gameOverOption("  Game over!\n", &mainMenu);
  gameOverOption = _gameOverOption;
  grOptsGo.push_back(&gameOverOption);
  Menu _gameOver(&grOptsGo, &lcd, true, 2000);
  gameOver = _gameOver;


  currentMenu = &greetingsMenu;
}

void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  lcd.begin(16, 2);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);

  pinMode(btPin, INPUT_PULLUP);

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, contrast);
  analogWrite(brightnessPin, brightness);
  
  player = Player(3, 10, 2, 14, &world);
  world = World(&lc, &player);
  createMenus();
  currentState = BrowsingMenus;
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
  if(currentMenu == &playStats && currentState == BrowsingMenus) {
    currentState = PlayingGame;
  }
  else if(currentMenu != &playStats && currentState == PlayingGame) {
    currentState = BrowsingMenus;
  } 

  if(currentState == PlayingGame && player.getLives() <= 0) {
    currentState = BrowsingMenus;
    currentMenu->clear();
    player.clear(3, 10, 2, 14);
    currentMenu = &gameOver;
  }

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
  currentMenu->drawMenu();
  currentMenu->checkDisplayValues();
  currentMenu->blinkCursor();
  if(currentState == PlayingGame) {
    if(millis() - Player::lastFell >= Player::fallInterval) {
      player.fall(); // check if they should fall
      Player::lastFell = millis();
    }
  }
  handleJoyInputs();
  handleJoyClick();
  if(currentMenu->isGreeting()) 
    currentMenu->killSelf(&currentMenu, &mainMenu);

//   Serial.println(hello->getOptions());
}
