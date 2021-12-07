#include "Arduino.h"
#include "Menu.h"

Menu::Menu() {
    Serial.println("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahewdewfewfvwfwfewfew");
    this->finsihedDrawing = false;
    Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
}

Menu::Menu(Vector<Option*> options, LiquidCrystal* lcd, bool greetingMenu = false) {
    Serial.println("hewdewfewfvwfwfew22121212121121few");
    this->options = options;
    this->lcd = lcd;
    this->cursor_x = this->cursor_y = this->o_x = this->o_y = 0;
    this->greetingMenu = greetingMenu;
    this->currentOption = 0;
    this->finsihedDrawing = false;
    Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
}

Menu::Menu(const Menu& other) {
    Serial.println("hewdewfedafdsfdsafdewfqfqefqewfvwfwfewfew");
    this->options = other.options;
    this->lcd = other.lcd;
    this->greetingMenu = other.greetingMenu;
    this->currentOption = other.currentOption;
    this->finsihedDrawing = false;
    Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
}

Menu& Menu::operator=(const Menu& other) {
    Serial.println("hewdewfewfvwfwfewfew");
    this->options = other.options;
    this->lcd = other.lcd;
    this->greetingMenu = other.greetingMenu;
    this->currentOption = other.currentOption;
    this->finsihedDrawing = false;
    Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
    return *this;
}

int Menu::getArduinoLine(int line, char* writeHere) {
    int currentLine = 0;
    size_t i;
    for(i = 0; i < this->options.size() && currentLine != line; ++i) {
        char optionText[MAX_OPT_TEXT];
        Option* crOption = this->options[i];
        crOption->getTextValue(optionText);
        for(size_t j = 0; optionText[j]; ++j) {
            if(optionText[j] == '\n') {
                currentLine += 1;
            }
        }
    }

    // now i points to the first word on the corresponding line
    char rawText[MAX_OPT_TEXT];
    size_t j = 0;
    for(; i < this->options.size(); ++i) {
        char optionText[MAX_OPT_TEXT];
        Option* crOption = this->options[i];
        crOption->getTextValue(optionText);
        bool last = false;
        // Serial.print(i);
        // Serial.println(" i pana acum");
        for(size_t k = 0; optionText[k]; ++k) {
            rawText[j++] = optionText[k];
            if(optionText[k] == '\n') 
                last = true;
        }
        if(j < 15) {
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
    // Serial.println(rawText);
    return j - 2;
}

// what draw menu basically does is animate a neat drawing animation and never do anything else afterwise
void Menu::drawMenu() {
    // Serial.println(this->finsihedDrawing ? "HIGH" : "LOW");
    if(this->finsihedDrawing)
        return;
    
    char lineText[MAX_OPT_TEXT];
    int lineSize = this->getArduinoLine(this->currentLine, lineText);
    if(lineSize <= 0) {
        this->finsihedDrawing = true;
        return;
    }

    if(millis() - this->lastLetterDrawn >= this->drawInterval) {
        Serial.println("new line");
        Serial.println(lineText);
        Serial.println(lineSize);
        this->lcd->setCursor(this->currentPos, this->currentLine);
        char printChar = lineText[this->currentPos];
        Serial.println(printChar);
        this->lcd->print(printChar);
        this->currentPos += 1;
        if(this->currentPos >= lineSize) {
            this->currentPos = 0;
            this->currentLine += 1;
            if(this->currentLine == this->firstLineShown + 2) {
                this->finsihedDrawing = true;
            }
        }
        this->lastLetterDrawn = millis();
    }
    // if(this->currentLine >= 2) {
    //     Serial.println("bruhino");
    //     return; // everything was already drawn
    // }

    // int currentPosition = this->currentLine * 16 + this->currentPos;
    // if(currentPosition >= this->options_raw_text_len) {
    //     Serial.println("bruh");
    //     return;
    // }

    // if(millis() - this->lastLetterDrawn >= this->drawInterval) {
    //     this->lcd->setCursor(this->currentPos, this->currentLine);
    //     char printChar = this->options_raw_text[currentPosition];
    //     Serial.println("point one");
    //     this->lcd->print(printChar);
    //     Serial.println("point two");
    //     this->currentPos += 1;
    //     if(this->currentPos >= 16) {
    //         this->currentPos = 0;
    //         this->currentLine += 1;
    //     }
    //     this->lastLetterDrawn = millis();
    // }
    // Serial.println("finished function call");
}