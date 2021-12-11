#include "Arduino.h"
#include "Menu.h"

const int Menu::cursorBlink = 500;
const byte Menu::drawInterval = 40;
long Menu::lastCursorBlink = 0;
bool Menu::blinkState = LOW;
const int Menu::upDownBlink = 500;
long Menu::lastUpDownBlink = 0;
bool Menu::upDownState = LOW;


Menu::Menu() {
    this->finsihedDrawing = false;
    this->optionSelected = 0;
}

Menu::Menu(Vector<Option*>* options, LiquidCrystal* lcd, bool greetingMenu = false, int timeDrawn = 0) {
    this->options = options;
    this->lcd = lcd;
    this->optionSelected = 0;
    this->greetingMenu = greetingMenu;
    this->finsihedDrawing = false;
    this->timeDrawn = timeDrawn;
}

Menu::Menu(const Menu& other) {
    this->options = other.options;
    this->optionSelected = 0;
    this->lcd = other.lcd;
    this->greetingMenu = other.greetingMenu;
    this->finsihedDrawing = false;
    this->timeDrawn = other.timeDrawn;
}

Menu& Menu::operator=(const Menu& other) {
    this->options = other.options;
    this->optionSelected = 0;
    this->lcd = other.lcd;
    this->greetingMenu = other.greetingMenu;
    this->finsihedDrawing = false;
    this->timeDrawn = other.timeDrawn;
    return *this;
}

int Menu::getArduinoLine(int line, char* writeHere) {
    int currentLine = 0;
    size_t i;
    for(i = 0; i < this->options->size() && currentLine != line; ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        for(size_t j = 0; optionText[j]; ++j) {
            if(optionText[j] == '\n') {
                currentLine += 1;
            }
        }
    }

    // now i points to the first word on the corresponding line
    char rawText[MAX_OPTION_TEXT];
    size_t j = 0;
    for(; i < this->options->size(); ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        bool last = false;
        // Serial.print(i);
        // Serial.println(" i pana acum");
        for(size_t k = 0; optionText[k]; ++k) {
            rawText[j++] = optionText[k];
            if(optionText[k] == '\n') 
                last = true;
        }
        if(j < 16 && !last) {
            rawText[j++] = ' ';
        }
        if(last)
            break;
    }
    rawText[j] = '\0';

    // maybe add spacing in the future
    for(size_t i = 0; rawText[i]; ++i) {
        writeHere[i] = rawText[i];
    }
    writeHere[j] = '\0';
    return j - 2;
}

// what draw menu basically does is animate a neat drawing animation and never do anything else afterwise
void Menu::drawMenu() {
    // Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
    if(this->finsihedDrawing)
        return;
    
    char lineText[MAX_OPTION_TEXT];
    int lineSize = this->getArduinoLine(this->currentLine, lineText);
    if(lineSize <= 0) {
        this->finsihedDrawing = true;
        return;
    }

    if(millis() - this->lastLetterDrawn >= this->drawInterval || (this->firstLineShown != 0 && this->currentLine == this->firstLineShown)) {
        this->lcd->setCursor(this->currentPos, this->currentLine - this->firstLineShown);
        char printChar = lineText[this->currentPos - 1];
        this->lcd->print(printChar);
        this->currentPos += 1;
        if(this->currentPos > lineSize + 1) {
            this->currentPos = 1;
            this->currentLine += 1;
            if(this->currentLine == this->firstLineShown + 2) {
                this->finsihedDrawing = true;
            }
        }
        this->lastLetterDrawn = millis();
    }
}

void Menu::clear() {
    this->lcd->clear();
    this->finsihedDrawing = 0;
    this->optionSelected = 0;
    this->firstLineShown = 0;
    this->currentLine = 0;
}

// for greetings menu, kill itself and transition to the next menu
void Menu::killSelf(Menu** currentMenu, Menu* nextMenu) {
    if(!this->greetingMenu)
        return;
    if(millis() - this->lastLetterDrawn > this->timeDrawn) {
        Option* currentOption = (*this->options)[this->optionSelected];
        currentOption->focus(currentMenu);
    }
}

Point Menu::findCursorPosition() {
    byte optionsLen = this->options->size();
    byte line = 0, col = 0;
    // line will point to the "line" in the options (they are delimitied by \n) and col will point to the last character before the current word
    // meaning that, after iterating through a word, it will be placed exactly where the blink cursor needs to be for the next word (and so on)
    for(size_t i = 0; i < optionsLen; ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        if(i == this->optionSelected) {
            return Point(col, line);
        }
        for(size_t k = 0; optionText[k]; ++k) {
            col++;
            if(optionText[k] == '\n') {
                col = 0;
                line += 1;
            }
        }
        if(col)
            col += 1;
    }
}

void Menu::blinkCursor() {
    if(!this->finsihedDrawing || this->greetingMenu)
        return;
    // stop the animation to show we locked in to this option


    Point cursorPos = this->findCursorPosition();
    byte lineOnLed = cursorPos.y - this->firstLineShown, colOnLed = cursorPos.x;

    if(this->checkOptionFocused()) {
        this->lcd->setCursor(colOnLed, lineOnLed);
        this->lcd->print(">");
        return;
    }

    if(millis() - Menu::lastCursorBlink >= Menu::cursorBlink) {
        this->lcd->setCursor(colOnLed, lineOnLed);
        if(!Menu::blinkState) {
            this->lcd->print(">");
            Menu::blinkState = HIGH;
        }
        else {
            this->lcd->print(" ");
            Menu::blinkState = LOW;
        }
        Menu::lastCursorBlink = millis();
    }
}

void Menu::blinkUpDown() {
    if(!this->finsihedDrawing || this->greetingMenu)
        return;
    
    if(this->firstLineShown == 0 && this->firstLineShown + 2 == this->getLastLine())
        return;

    if(millis() - Menu::lastUpDownBlink >= Menu::upDownBlink) {
        if(this->firstLineShown != 0) { 
            this->lcd->setCursor(15, 0);
            if(!Menu::upDownState)
                this->lcd->print("^");
            else
                this->lcd->print(" ");
        }
        if(this->firstLineShown + 2 != this->getLastLine()) {
            this->lcd->setCursor(15, 1);
            if(!Menu::upDownState) {
                this->lcd->print("v");
            }
            else {
                this->lcd->print(" ");
            }
        }
        Menu::upDownState = !Menu::upDownState;
        Menu::lastUpDownBlink = millis();
    }
}

// could possibly be more efficient to store this somewhere in the object? will see later
byte Menu::getLastLine() {
    byte line = 0;    
    byte optionsLen = this->options->size();
    for(size_t i = 0; i < optionsLen; ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        for(size_t k = 0; optionText[k]; ++k) {
            if(optionText[k] == '\n') {
                line += 1;
            }
        }
    }
    return line;
}

// should check whether an option is focused or not, in order to propagate joystick inputs to it
bool Menu::checkOptionFocused() {
    Option* currentOption = (*this->options)[this->optionSelected];
    return currentOption->isFocused();
}

byte Menu::logicalLineLength(byte line) {
    byte currentLine = 0;
    byte optionCount = 0;
    size_t i;
    for(i = 0; i < this->options->size(); ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        if(currentLine == line) {
            optionCount += 1;
        }
        for(size_t j = 0; optionText[j]; ++j) {
            if(optionText[j] == '\n' && currentLine != line) {
                currentLine += 1;
            }
            else if(optionText[j] == '\n') {
                return optionCount;
            }
        }
    }
}

byte Menu::optionLogPosInLine(byte line, Option* option) {
    byte currentLine = 0;
    byte optionCount = 0;
    size_t i;
    for(i = 0; i < this->options->size(); ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        if(currentLine == line) {
            optionCount += 1;
            if(crOption == option) {
                return optionCount;
            }
        }
        for(size_t j = 0; optionText[j]; ++j) {
            if(optionText[j] == '\n' && currentLine != line) {
                currentLine += 1;
            }
        }
    }
}

Option* Menu::getOptionAtLogPos(byte line, byte optPos) {
    byte currentLine = 0;
    byte optionCount = 0;
    size_t i;
    for(i = 0; i < this->options->size(); ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);
        if(currentLine == line) {
            optionCount += 1;
            if(optionCount == optPos) {
                return crOption;
            }
        }
        for(size_t j = 0; optionText[j]; ++j) {
            if(optionText[j] == '\n' && currentLine != line) {
                currentLine += 1;
            }
            else if(optionText[j] == '\n') {
                return crOption;
            }
        }
    }
}

byte Menu::getOptionVecPos(Option* option) {
    for(size_t i = 0; i < this->options->size(); ++i) {
        Option* currentOption = (*this->options)[i];
        if(currentOption == option) {
            return i;
        }
    }
}

void Menu::joystickInput(int xVal, int yVal) {
    // propagate input to option
    Option* currentOption = (*this->options)[this->optionSelected];
    if(this->checkOptionFocused()) {
        currentOption->joystickInput(xVal, yVal, this);
        return;
    }

    if(xVal || yVal) {
        Point cursorPos = this->findCursorPosition();
        byte lineOnLed = cursorPos.y - this->firstLineShown, colOnLed = cursorPos.x;
        this->lcd->setCursor(colOnLed, lineOnLed);
        this->lcd->print(" ");
        Menu::blinkState = LOW;
        Menu::lastCursorBlink = 0;
    }


    if(xVal == 1) {
        if(this->optionSelected != this->options->size() - 1) {
            this->optionSelected += 1;
        }
        else {
            this->optionSelected = 0;
        }
    }
    if(xVal == -1) {            
        if(this->optionSelected) {
            this->optionSelected -= 1;
        }
        else {
            this->optionSelected = this->options->size() - 1;
        }
    }

    if(yVal == 1) {
        Point cursorPos = this->findCursorPosition();
        byte line = cursorPos.y;
        byte crPos = this->optionLogPosInLine(line, currentOption);
        if(line == this->getLastLine() - 1) { // cant go further down than the last option
            byte nextLineLen = this->logicalLineLength(0);
            if(crPos > nextLineLen) {
                crPos = nextLineLen;
            }
            Option* newOption = this->getOptionAtLogPos(0, crPos);
            this->optionSelected = this->getOptionVecPos(newOption);
            Serial.println(line);
            Serial.println(crPos);
            Serial.println(nextLineLen);
            if(abs(line - 0) >= 2) {
                this->lcd->clear();
                this->currentLine = 0;
                this->firstLineShown = 0;
                this->currentPos = 1;
                this->finsihedDrawing = false;
            }
        }
        else {
            byte nextLineLen = this->logicalLineLength(line + 1);
            if(crPos > nextLineLen) {
                crPos = nextLineLen;
            }
            Option* newOption = this->getOptionAtLogPos(line + 1, crPos);
            this->optionSelected = this->getOptionVecPos(newOption);            
            Serial.println(line);
            Serial.println(crPos);
            Serial.println(nextLineLen);
            if(line + 1 == this->currentLine) {
                this->lcd->clear();
                this->firstLineShown = line;
                this->currentLine = line;
                this->currentPos = 1;
                this->finsihedDrawing = false;
            }
        }
    }
    else if(yVal == -1) {
        Point cursorPos = this->findCursorPosition();
        byte line = cursorPos.y;
        byte crPos = this->optionLogPosInLine(line, currentOption);
        if(line == 0) { // cant go further down than the last option
            byte nextLineLen = this->logicalLineLength(this->getLastLine() - 1);
            if(crPos > nextLineLen) {
                crPos = nextLineLen;
            }
            Option* newOption = this->getOptionAtLogPos(this->getLastLine() - 1, crPos);
            this->optionSelected = this->getOptionVecPos(newOption);      
            Serial.println("ma mananca palma stanga");      
            Serial.println(line);
            Serial.println(crPos);
            Serial.println(nextLineLen);
            Serial.println(this->getLastLine());
            if(this->getLastLine() - 2 != 0) {
                this->lcd->clear();
                this->currentLine = this->getLastLine() - 2;
                this->firstLineShown = this->getLastLine() - 2;
                this->finsihedDrawing = false;
                this->currentPos = 1;
            }
        }
        else {
            byte nextLineLen = this->logicalLineLength(line - 1);
            if(crPos > nextLineLen) {
                crPos = nextLineLen;
            }
            Option* newOption = this->getOptionAtLogPos(line - 1, crPos);
            this->optionSelected = this->getOptionVecPos(newOption);            
            Serial.println(line);
            Serial.println(crPos);
            Serial.println(nextLineLen);
            if(line - 1 < this->currentLine - 2) {
                this->lcd->clear();
                this->firstLineShown = line - 1;
                this->currentLine = line - 1;
                this->finsihedDrawing = false;
                this->currentPos = 1;
            }
        }
    }
}

void Menu::joystickClicked(Menu** currentMenu) {
    Option* currentOption = (*this->options)[this->optionSelected];
    if(currentOption->isFocused()) {
        currentOption->unfocus(); // unfocus on second click, it makes sense for options that change values
    }
    else {
        currentOption->focus(currentMenu);
    }
}

// use this to change displayed value when changing numerical values on an option, i.e. contrast
void Menu::updateOptionValue(Option* option) {
    byte line = 0, col = 1;
    for(size_t i = 0; i < this->options->size(); ++i) {
        char optionText[MAX_OPTION_TEXT];
        Option* crOption = (*this->options)[i];
        crOption->getTextValue(optionText);

        if(crOption == option) {
            this->lcd->setCursor(col, line - this->firstLineShown); // + 1 for the whitespace before it
            int len = strlen(optionText);
            if(optionText[len - 1] == '\n') {
                optionText[len - 1] = '\0';
            }
            this->lcd->print(optionText);
            return;
        }

        for(size_t k = 0; optionText[k]; ++k) {
            col++;
            if(optionText[k] == '\n') {
                col = 0;
                line += 1;
            }
        }
        col++;
    }
}

// check if display values have changed
void Menu::checkDisplayValues() {
    for(size_t i = 0; i < this->options->size(); ++i) {
        Option* crOption = (*this->options)[i];
        if(crOption->getType() != valueDisplay) // arduino can't use downcasting using dynamic cast
            continue;
        DisplayOption* d_crOption = (DisplayOption*) crOption;
        char optionText[MAX_OPTION_TEXT];
        crOption->getTextValue(optionText);
        // Serial.println(optionText);

        d_crOption->checkValue();
    }
}