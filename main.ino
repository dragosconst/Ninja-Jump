#include "LedControl.h"
#include <LiquidCrystal.h>
#include "Menu.h"

const int dinPin = 12;
const int clockPin = 8;
const int loadPin = 10;

const int xPin = A0;
const int yPin = A1;

const int contrastPin = 3;
const int RSPin = 13;
const int EPin = 9;
const int D4 = 7;
const int D5 = 6;
const int D6 = 4;
const int D7 = 2;
const int brightnessPin = 11;

int contrast = 50;
int brightness = 200;

const byte matrixSize = 8;

const int minThreshold = 200;
const int maxThreshold = 600;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;


LiquidCrystal lcd(RSPin, EPin, D4, D5, D6, D7);

Menu greetingsMenu, mainMenu, optionsMenu, aboutMenu, highScoreMenu, playStats;
GreetingOption welcomeMessage;
MenuOption playOption, settingsOption, aboutOption, highSchoreOption;
Option* grOptsArr[1];
Option* grOptsMnArr[4];
Vector<Option*> grOpts(grOptsArr), grOptsMn(grOptsMnArr);
Menu* currentMenu;

void createMenus() {
  const char* _welcomeText = "   Welcome! \n\0";
  GreetingOption _welcomeMessage(_welcomeText);
  welcomeMessage = _welcomeMessage;
  grOpts.push_back(&welcomeMessage);
  Menu menu(&grOpts, &lcd, true, 3000);
  greetingsMenu = menu;

  MenuOption _playOption(" Play", &playStats), _settingsOption("Settings\n", &optionsMenu), _aboutOption(" About", &aboutMenu), _highScoreOption("Scores\n", &highScoreMenu);
  playOption = _playOption;
  settingsOption = _settingsOption;
  aboutOption = _aboutOption;
  highSchoreOption = _highScoreOption;
  grOptsMn.push_back(&playOption); grOptsMn.push_back(&settingsOption); grOptsMn.push_back(&aboutOption); grOptsMn.push_back(&highSchoreOption);
  Menu _mainMenu(&grOptsMn, &lcd, false);
  mainMenu = _mainMenu;


  currentMenu = &greetingsMenu;
}

void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  lcd.begin(16, 2);

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, contrast);
  analogWrite(brightnessPin, brightness);
  
  createMenus();
}


void loop() {
   currentMenu->drawMenu();
   if(currentMenu->isGreeting()) 
    currentMenu->killSelf(&currentMenu, &mainMenu);
//   Serial.println(hello->getOptions());
}
