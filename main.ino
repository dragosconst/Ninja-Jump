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
int brightness = 750;

const byte matrixSize = 8;

const int minThreshold = 200;
const int maxThreshold = 600;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;


LiquidCrystal lcd(RSPin, EPin, D4, D5, D6, D7);

// Menu greetingsMenu, mainMenu, optionsMenu, aboutMenu, highScoreMenu;
Menu* hello;

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

  char* emptyVector[10];
  Vector<char*> greeting(emptyVector);
  

  char stuff[] = "stuff\0";
  lcd.begin(16, 2);
  greeting.push_back(stuff);
  Menu greetingsMenu(&greeting, nullptr, &lcd, true);
  Serial.println(greeting[0]);
  lcd.print(greeting[0]);
  hello = &greetingsMenu;
}


void loop() {
    Serial.println("looping");
   hello->drawMenu();
//   Serial.println(hello->getOptions());
}
