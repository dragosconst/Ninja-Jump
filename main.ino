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

int contrast = 80;
int brightness = 200;

const byte matrixSize = 8;

const int minThreshold = 200;
const int maxThreshold = 600;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;


LiquidCrystal lcd(RSPin, EPin, D4, D5, D6, D7);

Menu greetingsMenu, mainMenu, optionsMenu, aboutMenu, highScoreMenu;
GreetingOption welcomeMessage;
Option* grOptsArr[1];
Vector<Option*> grOpts(grOptsArr);
Menu* currentMenu;

void createMenus() {
  const char* _welcomeText = "   Welcome! \n\0";
  GreetingOption _welcomeMessage(_welcomeText);
  welcomeMessage = _welcomeMessage;
  grOpts.push_back(&welcomeMessage);
  Menu menu(grOpts, &lcd, true);
  greetingsMenu = menu;

  currentMenu = &greetingsMenu;
}

void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  pinMode(contrastPin, OUTPUT);
  pinMode(brightnessPin, OUTPUT);  
  analogWrite(contrastPin, contrast);
  analogWrite(brightnessPin, brightness);

  createMenus();
}


void loop() {
   currentMenu->drawMenu();
//   Serial.println(hello->getOptions());
}
