#include "Options.h"

Option::Option(OptionType type, char* text) {
    // text is assumed to be a correct value
    this->type = type;
    for(size_t i = 0; text[i]; ++i) {
        this->text[i] = text[i];
    }
}

MenuOption::MenuOption(char* text, Menu* nextMenu) : Option(menuTransition, text), nextMenu(nextMenu) {
}

void MenuOption::getTextValue(char* writeHere) {
    for(size_t i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
}

SystemOption::SystemOption(char* text, int pin, int baseValue, int stepValue) : Option(sysValue, text), pin(pin),
 baseValue(baseValue), stepValue(stepValue), currentStep(5), currentValue(baseValue) {

}

/// it's assumed xVal and yVal are either -1, 0 or 1
void SystemOption::joystickInput(int xVal, int yVal) {
    if(yVal == 1) {
        if(this->currentValue == this->baseValue + this->stepValue * 5) // maximum possible values
            return;
        this->currentValue += this->stepValue;
        analogWrite(this->pin, this->currentValue);
        this->currentStep += 1;
    }
    else if(yVal == -1) {
        if(this->currentValue == this->baseValue - this->stepValue * 5)
            return;
        this->currentValue -= this->stepValue;
        analogWrite(this->pin, this->currentValue);
        this->currentStep -= 1;
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
        writeHere[i++] = number[j];
    }
}

GameOption::GameOption(char* text, int* valAddr, int baseValue, int stepValue, int possibleSteps) : Option(gameValue, text), valAddr(valAddr), baseValue(baseValue),
stepValue(stepValue), currentValue(baseValue), possibleSteps(possibleSteps) { }

void GameOption::joystickInput(int xVal, int yVal) {
        if(yVal == 1) {
        if(this->currentValue == this->baseValue + this->possibleSteps / 2) // maximum possible values
            return;
        this->currentValue += this->stepValue;
        *this->valAddr = this->currentValue;
    }
    else if(yVal == -1) {
        if(this->currentValue == this->baseValue - this->possibleSteps / 2)
            return;
        this->currentValue -= this->stepValue;
        *this->valAddr = this->currentValue;
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
}

DisplayOption::DisplayOption(char* text, int* value) : Option(valueDisplay, text), value(value) {}

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
}

GreetingOption::GreetingOption(char* text) : Option(greeting, text) {}

void GreetingOption::getTextValue(char* writeHere) {
        size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
}
