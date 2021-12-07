#include "Arduino.h"
#include "Menu.h"

Menu::Menu(Vector<char*>* options, Vector<Menu*>* menus, LiquidCrystal* lcd, bool greetingMenu = false) {
    this->options = options;
    this->lcd = lcd;
    this->options_menus = menus;
    this->cursor_x = this->cursor_y = this->o_x = this->o_y = 0;
    this->greetingMenu = greetingMenu;
    size_t j = 0;
    for(size_t i = 0;  i < this->options->size(); ++i) {
        char* option = this->options->at(i);
        for(size_t k = 0; option[k] != '\0'; ++k) {
            this->options_raw_text[j++] = option[k];
        }
        Serial.println(i);
        Serial.println(option);
        Serial.println(this->options_raw_text); 
    }
    this->options_raw_text[j] = '\0';
    this->options_raw_text_len = j;
    Serial.println("buna seara buna seara");
}


void Menu::drawMenu() {
    if(this->currentLine >= 2) {
        Serial.println("bruhino");
        return; // everything was already drawn
    }

    int currentPosition = this->currentLine * 16 + this->currentPos;
    if(currentPosition >= this->options_raw_text_len) {
        Serial.println("bruh");
        return;
    }
    Serial.println("am venit cu harcoreala");
    delay(10);
    Serial.println(this->currentLine);
    delay(10);
    Serial.println(this->currentPos);
    delay(10);
    Serial.println(this->options_raw_text);
    delay(10);
    Serial.println(this->options_raw_text[currentPosition]);
    delay(10);

    if(millis() - this->lastLetterDrawn >= this->drawInterval) {
        this->lcd->setCursor(this->currentPos, this->currentLine);
        char printChar = this->options_raw_text[currentPosition];
        Serial.println("point one");
        this->lcd->print(printChar);
        Serial.println("point two");
        this->currentPos += 1;
        if(this->currentPos >= 16) {
            this->currentPos = 0;
            this->currentLine += 1;
        }
        this->lastLetterDrawn = millis();
    }
    Serial.println("finished function call");
}