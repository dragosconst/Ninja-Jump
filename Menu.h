#ifndef MENU_H
#define MENU_H

#define MAX_OPT_TEXT 100

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include "Options.h"

class Option;

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
public:
    static const byte cursorBlink = 70; // interval at which the cursor blinks
    static const byte drawInterval = 40; // interval at which to draw the menu

    Menu();
    Menu(Vector<Option*>* options, LiquidCrystal* lcd, bool greetingMenu, int timeDrawn=0);
    Menu(const Menu& other);
    Menu& operator=(const Menu& other);
    ~Menu() { }

    void drawMenu();
    void blinkCursor();
    void killSelf(Menu** currentMenu, Menu* nextMenu);

    bool isGreeting() const { return this->greetingMenu; }
};

#endif