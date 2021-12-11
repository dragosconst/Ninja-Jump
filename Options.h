#ifndef OPTION_H
#define OPTION_H
#define MAX_OPTION_TEXT 18

#include "Arduino.h"
#include <Vector.h>
#include "LiquidCrystal.h"
#include <LedControl.h>
#include "Menu.h"

// a menu option can either be:
// 1. a menu transition - selecting it will transition to a new menu
// 2. a system value changer - it will change a system value, like contrast
// 3. led matrix brightness - basically a singleton, controls the led brightness
// 4. a game value changer - it will change a value related to the game's logic, like difficulty
// 5. naming option for high scores 
// 6. a value display - this is used for displaying stuff in game, you can't actually do anything with these options but look at them
// 7. greeting - might be necessary for game over and welcome screens
enum OptionType { menuTransition, sysValue, gameValue, ledValue, nameOption, valueDisplay, greeting};

class Menu;

// Abstract class that serves as the building block for the option types. I've opted for this approach because having one big Option class with
// all relevant members for each type seemed like a huge waste of memory.
// !!!! For an option that's supposed to be the last option on a LCD line, make sure to add \n at the end.
class Option {
protected:
    OptionType type;
    char text[MAX_OPTION_TEXT];
    bool inFocus;
public:
    Option() {}
    Option(OptionType type, const char* text);
    virtual ~Option() { }

    bool isFocused() const { return this->inFocus; }
    OptionType getType() const { return this->type; }

    virtual void focus(Menu** currentMenu) = 0; // focused will execute the action expected when selecting the option
    virtual void joystickInput(int xVal, int yVal, Menu* currentMenu) = 0; // feeds input to the joystick
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

    void focus(Menu** currentMenu);
    void joystickInput(int xVal, int yVal, Menu* currentMenu) { } // this is just a menu transition, it won't do anything with joystick inputs
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
    bool last;
    void (*eepromUpdate)(int);
public:
    SystemOption() {}
    SystemOption(const char* text, int pin, int baseValue, int currentValue, int stepValue, bool last, void (*eepromUpdate)(int));
    ~SystemOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus() { this->inFocus = false; this->eepromUpdate(this->currentValue); }
    void getTextValue(char* writeHere);
};

class LEDOption : public Option {
private:
    LedControl* lc;
    int brightValue;
    void (*eepromUpdate)(int);
    bool last;

    void updateMatrix();
public:
    LEDOption() {}
    LEDOption(const char* text, LedControl* lc, int brightValue, bool last, void (*eepromUpdate)(int));
    ~LEDOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true; this->updateMatrix(); }
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus() { this->inFocus = false; this->eepromUpdate(this->brightValue); this->lc->clearDisplay(0);}
    void getTextValue(char* writeHere);
};

class GameOption : public Option {
private:
    int* valAddr; // address of the value we want to change
    int baseValue;
    int currentValue;
    int stepValue;
    int possibleSteps;
    bool last;
    void (*eepromUpdate)(int);
public:
    GameOption() {}
    GameOption(const char* text, int* valAddr, int baseValue, int currentValue, int stepValue, int possibleSteps, bool last, void (*eepromUpdate)(int));
    ~GameOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus() { this->inFocus = false;this->eepromUpdate(this->currentValue);}
    void getTextValue(char* writeHere);
};

class DisplayOption : public Option {
private:
    int* value; // using a pointer to the value so we don't have to bother with getters and setters
    int oldValue;
    bool last;
    Menu* currentMenu;
    long lastChecked;
public:
    static const byte checkInterval = 10; // interval at which the value is checked for changes
    
    DisplayOption() {}
    DisplayOption(const char* text, int* value, bool last, Menu* currentMenu);
    ~DisplayOption() {}

    void focus(Menu** currentMenu) {} // these won't ever fire, this class just displays a value
    void joystickInput(int xVal, int yVal, Menu* currentMenu) {} 
    void checkValue();
    void unfocus() {}
    void getTextValue(char* writeHere);
};

class GreetingOption : public Option {
public:
    GreetingOption() {}
    GreetingOption(const char* text);
    ~GreetingOption() {}

    void focus(Menu** currentMenu) {}
    void joystickInput(int xVal, int yVal, Menu* currentMenu) {} 
    void unfocus() {}
    void getTextValue(char* writeHere);
};
#endif