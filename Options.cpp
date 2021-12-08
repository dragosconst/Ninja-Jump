#include "Options.h"

Option::Option(const char* text) {
    // text is assumed to be a correct value
    size_t i;
    for(i = 0; text[i]; ++i) {
        this->text[i] = text[i];
    }
    this->text[i] = '\0';
    this->inFocus = false;
}

MenuOption::MenuOption(const char* text, Menu* nextMenu) : Option(text), nextMenu(nextMenu) {
}

void MenuOption::focus(Menu** currentMenu) {
    (*currentMenu)->clear(); 
    *currentMenu = nextMenu; 
    this->unfocus(); 
}

void MenuOption::getTextValue(char* writeHere) {
    size_t i;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    writeHere[i] = '\0';
}

SystemOption::SystemOption(const char* text, int pin, int baseValue, int stepValue, bool last) : Option(text), pin(pin),
 baseValue(baseValue), stepValue(stepValue), currentStep(5), currentValue(baseValue), last(last) {

}

/// it's assumed xVal and yVal are either -1, 0 or 1
void SystemOption::joystickInput(int xVal, int yVal, Menu* currentMenu) {
    if(yVal == -1) {
        if(this->currentValue == this->baseValue + this->stepValue * 5) // maximum possible values
            return;
        this->currentValue += this->stepValue;
        analogWrite(this->pin, this->currentValue);
        this->currentStep += 1;
        currentMenu->updateOptionValue(this);
    }
    else if(yVal == 1) {
        if(this->currentValue == this->baseValue - this->stepValue * 5)
            return;
        this->currentValue -= this->stepValue;
        analogWrite(this->pin, this->currentValue);
        this->currentStep -= 1;
        currentMenu->updateOptionValue(this);
    }
}

void SystemOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa(this->currentStep, number, 10);
    for(size_t j = 0; number[j]; ++j) {
        if(this->currentStep < 10 && j == 0) {
            writeHere[i++] = ' ';
        }
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}

GameOption::GameOption(const char* text, int* valAddr, int baseValue, int stepValue, int possibleSteps, bool last) : Option(text), valAddr(valAddr), baseValue(baseValue),
stepValue(stepValue), currentValue(baseValue), possibleSteps(possibleSteps), last(last) { }

void GameOption::joystickInput(int xVal, int yVal, Menu* currentMenu) {
    if(yVal == -1) {
        if(this->currentValue == this->baseValue + this->possibleSteps / 2) // maximum possible values
            return;
        this->currentValue += this->stepValue;
        *this->valAddr = this->currentValue;
        currentMenu->updateOptionValue(this);
    }
    else if(yVal == 1) {
        if(this->currentValue == this->baseValue - this->possibleSteps / 2)
            return;
        this->currentValue -= this->stepValue;
        *this->valAddr = this->currentValue;
        currentMenu->updateOptionValue(this);
    }
}

void GameOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa(this->currentValue, number, 10);
    for(size_t j = 0; number[j]; ++j) {
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}

DisplayOption::DisplayOption(const char* text, int* value) : Option(text), value(value) {}

void DisplayOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa(*this->value, number, 10);    
    for(size_t j = 0; number[j]; ++j) {
        writeHere[i++] = number[j];
    }
    writeHere[i] = '\0';
}

GreetingOption::GreetingOption(const char* text) : Option(text) {}

void GreetingOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    writeHere[i] = '\0';
}
