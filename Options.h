#ifndef OPTION_H
#define OPTION_H
#define MAX_OPTION_TEXT = 17

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include "Menu.h"

// a menu option can either be:
// 1. a menu transition - selecting it will transition to a new menu
// 2. a system value changer - it will change a system value, like contrast
// 3. a game value changer - it will change a value related to the game's logic, like difficulty
// 4. a value display - this is used for displaying stuff in game, you can't actually do anything with these options but look at them
// 5. greeting - might be necessary for game over and welcome screens
enum OptionType { menuTransition, sysValue, gameValue, valueDisplay, greeting};

class Menu;

// Abstract class that serves as the building block for the option types. I've opted for this approach because having one big Option class with
// all relevant members for each type seemed like a huge waste of memory.
// !!!! For an option that's supposed to be the last option on a LCD line, make sure to add \n at the end.
class Option {
protected:
    OptionType type;
    char text[MAX_OPT_TEXT];
    bool inFocus = false;
public:
    Option() {}
    Option(OptionType type, const char* text);
    ~Option() { }

    virtual void focus(Menu* currentMenu) = 0; // focused will execute the action expected when selecting the option
    virtual void joystickInput(int xVal, int yVal) = 0; // feeds input to the joystick
    virtual void unfocus() = 0; // makes sense for value changing menus
    virtual void getTextValue(char* writeHere) = 0;
};

class MenuOption : public Option {
private:
    Menu* nextMenu;
public:
    MenuOption() {}
    MenuOption(const char* text, Menu* nextMenu);
    ~MenuOption() { }

    void focus(Menu* currentMenu) { currentMenu = nextMenu; this->unfocus(); }
    void joystickInput(int xVal, int yVal) { } // this is just a menu transition, it won't do anything with joystick inputs
    void unfocus() { this->inFocus = false;}
    void getTextValue(char* writeHere);
};

class SystemOption : public Option {
private:
    int pin;
    int baseValue;
    int currentValue;
    int stepValue;
    int currentStep;
public:
    SystemOption() {}
    SystemOption(const char* text, int pin, int baseValue, int stepValue);
    ~SystemOption() {}

    void focus(Menu* currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal);
    void unfocus() { this->inFocus = false;}
    void getTextValue(char* writeHere);
};

class GameOption : public Option {
private:
    int* valAddr; // address of the value we want to change
    int baseValue;
    int currentValue;
    int stepValue;
    int possibleSteps;
public:
    GameOption() {}
    GameOption(const char* text, int* valAddr, int baseValue, int stepValue, int possibleSteps);
    ~GameOption() {}

    void focus(Menu* currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal);
    void unfocus() { this->inFocus = false;}
    void getTextValue(char* writeHere);
};

class DisplayOption : public Option {
private:
    int* value; // using a pointer to the value so we don't have to bother with getters and setters
public:
    DisplayOption() {}
    DisplayOption(const char* text, int* value);
    ~DisplayOption() {}

    void focus(Menu* currentMenu) {} // these won't ever fire, this class just displays a value
    void joystickInput(int xVal, int yVal) {} 
    void unfocus() {}
    void getTextValue(char* writeHere);
};

class GreetingOption : public Option {
public:
    GreetingOption() {}
    GreetingOption(const char* text);
    ~GreetingOption() {}

    void focus(Menu* currentMenu) {}
    void joystickInput(int xVal, int yVal) {} 
    void unfocus() {}
    void getTextValue(char* writeHere);
};
#endif