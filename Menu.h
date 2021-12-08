#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include "Options.h"

class Option;

struct Point {
    int x, y;
    Point(int x, int y) {this->x = x; this->y = y;}
};

class Menu {
private:
    Vector<Option*>* options;
    byte optionSelected;
    long lastLetterDrawn = 0; // time at which the last letter was drawn
    byte currentLine = 0; // current line being drawn
    byte currentPos = 0; // current position on current line
    byte firstLineShown = 0; // for menus that have more than just two lines
    bool greetingMenu;
    LiquidCrystal* lcd;
    bool finsihedDrawing;
    int timeDrawn;

    int getArduinoLine(int line, char* writeHere);
    byte getLastLine();
    Point findCursorPosition();
public:
    static const int cursorBlink; // interval at which the cursor blinks
    static const byte drawInterval; // interval at which to draw the menu
    static long lastCursorBlink; // we don't really need to store this for each menu
    static bool blinkState;

    Menu();
    Menu(Vector<Option*>* options, LiquidCrystal* lcd, bool greetingMenu, int timeDrawn=0);
    Menu(const Menu& other);
    Menu& operator=(const Menu& other);
    ~Menu() { }

    void drawMenu();
    void blinkCursor();
    void killSelf(Menu** currentMenu, Menu* nextMenu);
    void joystickInput(int xVal, int yVal);
    void joystickClicked();

    bool isGreeting() const { return this->greetingMenu; }
};

#endif