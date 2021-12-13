#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include "Options.h"

enum GameStates {BrowsingMenus, PlayingGame};
class Option;

struct Point {
    int x, y;
    Point(int x, int y) {this->x = x; this->y = y;}
};


/**
 * @brief 
 * Small notes about the Menu class.
 * I've made the distinction between logical and physical positions for lines and options in the Menu. Logical positions mean things like which
 * option this one is (first, second etc.) or which line this one is. Logical size applies to lines and means how many options the line contains.
 * Physical position refers to the position of a character in the char array representing the lines (and therefore an option's physical position
 * is where it starts in the array) and physical size refers to the lines length in bytes.
 */
class Menu {
private:
    Vector<Option*>* options;
    byte optionSelected;
    unsigned long lastLetterDrawn; // time at which the last letter was drawn
    byte currentLine; // current line being drawn
    byte currentPos; // current position on current line -> this is actual array position, not logical position, i.e. which option this one is in its respective line
    byte firstLineShown; // for menus that have more than just two lines
    bool greetingMenu;
    bool playingMenu;
    LiquidCrystal* lcd;
    bool finsihedDrawing;
    int timeDrawn;

    int getArduinoLine(int line, char* writeHere);
    byte getLastLine();
    byte optionLogPosInLine(byte line, Option* option); // logical position of option in line: whether it's the first, second etc.
    byte logicalLineLength(byte line); // logical line length = how many options the line contains
    Option* getOptionAtLogPos(byte line, byte optPos);
    byte getOptionVecPos(Option* option); // get position in original vector of options of a given option
    Point findCursorPosition();
    bool checkOptionFocused();
public:
    static const int cursorBlink; // interval at which the cursor blinks
    static const byte drawInterval; // interval at which to draw the menu
    static unsigned long lastCursorBlink; // we don't really need to store this for each menu
    static bool blinkState;

    static const int upDownBlink; // interval at which the up and down indicators blink
    static unsigned long lastUpDownBlink;
    static bool upDownState;

    Menu();
    Menu(Vector<Option*>* options, LiquidCrystal* lcd, bool greetingMenu, int timeDrawn=0, bool playinfgMenu=false);
    Menu(const Menu& other);
    Menu& operator=(const Menu& other);
    ~Menu() {}

    void drawMenu();
    void clear();
    void blinkCursor();
    void blinkUpDown();
    void killSelf(Menu** currentMenu);
    void joystickInput(int xVal, int yVal);
    void joystickClicked(Menu** currentMenu);
    void updateOptionValue(Option* option);
    void checkDisplayValues();
    void freeOptions();
    void printValues();
    void updateState(GameStates* state);

    bool isGreeting() const { return this->greetingMenu; }
};

#endif