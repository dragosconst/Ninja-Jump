#ifndef OPTION_H
#define OPTION_H

#define MAX_OPTION_TEXT 18
#define VOL_0 25
#define NORMAL 1
#define HARD 2

#define ALPH_SIZE 65
#define NAME_LIMIT 3
#define LETTER_FLASH 600

#include "Arduino.h"
#include "LiquidCrystal.h"
#include <LedControl.h>
#include "Menu.h"
#include "Player.h"
#include "RWHelper.h"

// a menu option can either be:
// 1. a menu transition - selecting it will transition to a new menu
// 2. a system value changer - it will change a system value, like contrast
// 3. led matrix brightness - basically a singleton, controls the led brightness
// 4. a game value changer - it will change a value related to the game's logic, like difficulty
// 5. naming option for high scores - names will have only 3 characters for EEPROM's sake -> could modify in the future
// 6. a value display - this is used for displaying stuff in game, you can't actually do anything with these options but look at them
// 7. greeting - might be necessary for game over and welcome screens
// 8. volume option for the music
// 9. change in-game music theme
// 10. turn off sounds
enum OptionType { menuTransition, sysValue, gameValue, ledValue, nameOption, valueDisplay, greeting, volumeOption, themeOption, soundOption};

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
    Menu* (*createMenu)(void);
public:
    MenuOption() {}
    MenuOption(const char* text, Menu* (*createMenu)(void));
    MenuOption(const MenuOption& other);
    MenuOption& operator=(const MenuOption& other);
    ~MenuOption() { }

    void focus(Menu** currentMenu);
    void joystickInput(int xVal, int yVal, Menu* currentMenu) { } // this is just a menu transition, it won't do anything with joystick inputs
    void unfocus() { this->inFocus = false;}
    void getTextValue(char* writeHere);
};

class SystemOption : public Option {
private:
    byte pin;
    byte baseValue;
    byte currentValue;
    byte stepValue;
    void (*eepromUpdate)(byte);
public:
    SystemOption() {}
    SystemOption(const char* text, byte pin, byte baseValue, byte currentValue, byte stepValue, void (*eepromUpdate)(byte));
    ~SystemOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus() { this->inFocus = false; this->eepromUpdate(this->currentValue); }
    void getTextValue(char* writeHere);
};

class LEDOption : public Option {
private:
    LedControl* lc;
    byte brightValue;

    void updateMatrix();
public:
    LEDOption() {}
    LEDOption(const char* text, LedControl* lc, byte brightValue);
    ~LEDOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true; this->updateMatrix(); }
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus() { this->inFocus = false; RWHelper::writeByte(LED_ADDR,this->brightValue); this->lc->clearDisplay(0);}
    void getTextValue(char* writeHere);
};

class GameOption : public Option {
private:
    int* valAddr; // address of the value we want to change
    byte minVal;
    byte maxVal;
    byte currentValue;

    byte transformVolume() const { return this->currentValue + VOL_0;}
public:
    GameOption() {}
    GameOption(const char* text, int* valAddr, byte minVal, byte maxVal, byte currentValue, OptionType type);
    ~GameOption() {}

    void focus(Menu** currentMenu) { this->inFocus = true;}
    void joystickInput(int xVal, int yVal, Menu* currentMenu);  
    void unfocus();
    void getTextValue(char* writeHere);
};

// this also acts as sort of a pseudo-Menu option, beacause I thought it would be better to instantly transition after inputting your name
class NameOption : public Option {
private:
    static const char alphabet[ALPH_SIZE];
    int score;
    byte vals[NAME_LIMIT];
    char name[NAME_LIMIT + 1];
    Menu* (*nextMenu)(void);
    Menu** currentMenu;
    byte crIndex; // which letter i have selected at this moment
    byte crChar; // which character of the name im currently writing
    bool updatedIndex;
    long lastFlash;
    bool flashState;
public:
    NameOption() {}
    NameOption(const char* text, int score, Menu* (*nextMenu)(void));
    ~NameOption() {}

    void focus(Menu** currentMenu);
    void joystickInput(int xVal, int yVal, Menu* currentMenu);
    void unfocus();
    bool changedChar();
    void getTextValue(char* writeHere);
};

class DisplayOption : public Option {
private:
    int* value; // using a pointer to the value so we don't have to bother with getters and setters
    byte oldValue;
    Menu* currentMenu;
    long lastChecked;
public:
    static const byte checkInterval; // interval at which the value is checked for changes
    
    DisplayOption() {}
    DisplayOption(const char* text, int* value, Menu* currentMenu);
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