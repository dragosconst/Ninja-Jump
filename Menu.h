#ifndef MENU_H
#define MENU_H

#define MAX_OPT_TEXT 100

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include "Options.h"

class Menu {
private:
    Vector<char*>* options;
    char options_raw_text[MAX_OPT_TEXT];
    int options_raw_text_len;
    Vector<Menu*>* options_menus; // the menus that the options point to
    int cursor_x, cursor_y; // positions of player cursor on the menu
    int o_x, o_y; // to which option the cursor points logically, not its actual position on the menu
    long lastLetterDrawn = 0; // time at which the last letter was drawn
    byte currentLine = 0; // current line being drawn
    byte currentPos = 0; // current position on current line
    bool greetingMenu;
    LiquidCrystal* lcd;
public:
    static const int cursorBlink = 70; // interval at which the cursor blinks
    static const int drawInterval = 40; // interval at which to draw the menu

    Menu(Vector<char*>* options, Vector<Menu*>* menus, LiquidCrystal* lcd, bool greetingMenu);
    ~Menu() { }

    void drawMenu();
    const char* getOptions() const { return this->options_raw_text;}
    void blinkCursor();
};

#endif