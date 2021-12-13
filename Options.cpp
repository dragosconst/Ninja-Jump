#include "Options.h"

Option::Option(OptionType type, const char* text) {
    // text is assumed to be a correct value
    size_t i;
    for(i = 0; text[i]; ++i) {
        this->text[i] = text[i];
    }
    this->text[i] = '\0';
    this->inFocus = false;
    this->type = type;
}

MenuOption::MenuOption(const char* text, Menu* (*createMenu)(void)) : Option(menuTransition, text),
createMenu(createMenu) {
}

MenuOption::MenuOption(const MenuOption& other) {
    this->type = other.type;
    int i;
    for(i = 0; other.text[i]; ++i) {
        this->text[i] = other.text[i];
    }
    this->text[i] = '\0';
    this->createMenu = other.createMenu;
}

MenuOption& MenuOption::operator=(const MenuOption& other) {
    this->type = other.type;
    int i;
    for(i = 0; other.text[i]; ++i) {
        this->text[i] = other.text[i];
    }
    this->text[i] = '\0';
    // Serial.println(text);
    this->createMenu = other.createMenu;
    return *this;
}

void MenuOption::focus(Menu** currentMenu) {
    Menu* oldMenu = *currentMenu;
    // Serial.println("this is good so far");
    // delay(300);
    (*currentMenu)->clear();
    *currentMenu = this->createMenu();     

    /**
     * @brief 
     * So obviously, in a regular environment this would not work. We are essentially freeing this option with the freeOptions call.
     * However, since Arduino doesn't have an OS and therefore there's no other process running, the free operation will not actually
     * result in they bytes being changed to anything, so the program keeps on executing them, even if they aren't technically part of the
     * address space anymore.
     * This might be unstable if Arduino has illegal memory access errors, but it doesn't seem to have any.
     */
    oldMenu->freeOptions();
    delete oldMenu;
    // this->unfocus(); 
}

void MenuOption::getTextValue(char* writeHere) {

    size_t i;
    for(i = 0; this->text[i]; ++i){
        // Serial.println(this->text[i]);
        writeHere[i] = this->text[i];
        // Serial.println(this->text[i]);
        // delay(300);
    }
    writeHere[i] = '\0';
}

SystemOption::SystemOption(const char* text, byte pin, byte baseValue, byte currentValue, byte stepValue, bool last, void (*eepromUpdate)(byte)) : Option(sysValue, text), pin(pin),
 baseValue(baseValue), stepValue(stepValue), currentValue(currentValue), last(last), eepromUpdate(eepromUpdate) {

}


/// it's assumed xVal and yVal are either -1, 0 or 1
void SystemOption::joystickInput(int xVal, int yVal, Menu* currentMenu) {
    if(yVal == -1) {
        if(this->currentValue == this->baseValue + this->stepValue * 5) // maximum possible values
            return;
        this->currentValue += this->stepValue;
        analogWrite(this->pin, this->currentValue);
        currentMenu->updateOptionValue(this);
    }
    else if(yVal == 1) {
        if(this->currentValue == this->baseValue - this->stepValue * 5)
            return;
        this->currentValue -= this->stepValue;
        analogWrite(this->pin, this->currentValue);
        currentMenu->updateOptionValue(this);
    }
}

void SystemOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa(5 - (baseValue - currentValue) / stepValue, number, 10);
    for(size_t j = 0; number[j]; ++j) {
        if(5 - (baseValue - currentValue) / stepValue < 10 && j == 0) {
            writeHere[i++] = ' ';
        }
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}


LEDOption::LEDOption(const char* text, LedControl* lc, byte brightValue, bool last, void (*eepromUpdate)(byte)) : Option(ledValue, text), lc(lc),
brightValue(brightValue), last(last), eepromUpdate(eepromUpdate) { }

void LEDOption::joystickInput(int xVal, int yVal, Menu* currentMenu) {
    if(yVal == -1) {
        if(this->brightValue == 15)
            return;
        this->brightValue += 1;
        this->updateMatrix();
        currentMenu->updateOptionValue(this);
    }
    else if(yVal == 1) {
        if(this->brightValue == 0)
            return;
        this->brightValue -= 1;
        this->updateMatrix();
        currentMenu->updateOptionValue(this);
    }
}

void LEDOption::updateMatrix() {
    this->lc->setIntensity(0, this->brightValue);
    for(int i = 0; i < 8; ++i) {
        this->lc->setRow(0, i, B11111111);
    }
}

void LEDOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa(this->brightValue, number, 10);
    for(size_t j = 0; number[j]; ++j) {
        if(this->brightValue < 10 && j == 0) {
            writeHere[i++] = ' ';
        }
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}

GameOption::GameOption(const char* text, int* valAddr, byte baseValue, byte currentValue, byte stepValue, byte possibleSteps, bool last, void (*eepromUpdate)(byte)) : Option(gameValue, text), valAddr(valAddr), baseValue(baseValue),
stepValue(stepValue), currentValue(currentValue), possibleSteps(possibleSteps), last(last), eepromUpdate(eepromUpdate) { }

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
        if(this->currentValue < 10 && j == 0) {
            writeHere[i++] = ' ';
        }
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}


const char NameOption::alphabet[62] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

NameOption::NameOption(const char* text, Player* player, void (*eepromUpdate)(int, char*), Menu* (*nextMenu)(void)) : Option(nameOption, text), player(player),
eepromUpdate(eepromUpdate), crChar(0), crIndex(10), updatedIndex(false), nextMenu(nextMenu) {
    for(byte j = 0; j < 3; ++j) {
        this->vals[j] = 10;
    }
}

void NameOption::focus(Menu** currentMenu) {
    this->inFocus = true;
    this->currentMenu = currentMenu;
}

void NameOption::unfocus() {
    this->name[this->crChar] = NameOption::alphabet[this->crIndex];
    this->crIndex = this->vals[++crChar];
    if(crChar == 3) {
        this->inFocus = false;
        this->name[3] == '\0';
        Serial.println(this->player->getHeight());
        this->eepromUpdate(this->player->getHeight(), this->name);
        this->player->clear(3, 10, 2, 14);
        Menu* oldMenu = *currentMenu;
        // Serial.println("this is good so far");
        // delay(300);
        (*currentMenu)->clear();
        *currentMenu = this->nextMenu();
        oldMenu->freeOptions();
        delete oldMenu;
    }
}

void NameOption::joystickInput(int xVal, int yVal, Menu* currentMenu) {
    if(yVal == -1) {
        this->updatedIndex = true;
        if(this->crIndex > 0) {
            this->crIndex--;
        }
        else {
            this->crIndex = 61;
        }
        this->vals[crChar] = this->crIndex;
    }
    else if(yVal == 1) {
        this->updatedIndex = true;
        if(this->crIndex < 61) {
            this->crIndex++;
        }
        else {
            this->crIndex = 0;
        }
        this->vals[crChar] = this->crIndex;
    }
}

bool NameOption::changedChar() {
    bool retval = this->updatedIndex;
    this->updatedIndex = false;
    return retval;
}

void NameOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    for(byte j = 0; j < 3; ++j) {
        writeHere[i++] = NameOption::alphabet[this->vals[j]];
    }
    writeHere[i++] = '\n'; // name option will always be treated as last name option
    writeHere[i] = '\0';
}

DisplayOption::DisplayOption(const char* text, int* value, bool last, Menu* currentMenu) : Option(valueDisplay, text), value(value), oldValue(*value), last(last), currentMenu(currentMenu) {}

long DisplayOption::lastChecked = 0;

void DisplayOption::checkValue() {
    if(millis() - DisplayOption::lastChecked >= DisplayOption::checkInterval) {
        if(this->oldValue != (*this->value)) {
            this->oldValue = *this->value;
            this->currentMenu->updateOptionValue(this);
        }
        this->lastChecked = millis();
    }
}

void DisplayOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    char number[20];
    itoa((*this->value), number, 10);    
    for(size_t j = 0; number[j]; ++j) {
        writeHere[i++] = number[j];
    }
    if(this->last)
        writeHere[i++] = '\n';
    writeHere[i] = '\0';
}

GreetingOption::GreetingOption(const char* text) : Option(greeting, text) {}

void GreetingOption::getTextValue(char* writeHere) {
    size_t i = 0;
    for(i = 0; this->text[i]; ++i){
        writeHere[i] = this->text[i];
    }
    writeHere[i] = '\0';
}
