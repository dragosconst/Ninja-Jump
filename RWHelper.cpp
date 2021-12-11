#include "Arduino.h"
#include "RWHelper.h"

const byte RWHelper::contrastAddr = 0;
const byte RWHelper::LCDbrightAddr = 1;
const byte RWHelper::LEDbrightAddr = 2;
const byte RWHelper::diffAddr = 3;
const byte RWHelper::highScoreNumAddr = 4; // from 4 onwards the highscores will be stored
const byte RWHelper::charLimit = 3;

void RWHelper::clear() {
    for(int i = 0; i < 10; ++i) {
        EEPROM.put(i, 0);
    }
}

void RWHelper::writeContrast(int value) {
    EEPROM.put(RWHelper::contrastAddr, value);
}

void RWHelper::writeLCDBright(int value) {
    EEPROM.put(RWHelper::LCDbrightAddr, value);
}

void RWHelper::writeLEDBright(int value) {
    EEPROM.put(RWHelper::LEDbrightAddr, value);
}

void RWHelper::writeDiff(int value) {
    EEPROM.put(RWHelper::diffAddr, value);
}

void RWHelper::writeHighNum(int value) {
    EEPROM.put(RWHelper::highScoreNumAddr, value);
}

void RWHelper::writeHigh(int value, char* name) {
    byte highScoreSize = 1 + RWHelper::charLimit;
    int n;
    EEPROM.get(RWHelper::highScoreNumAddr, n);
    int oldValue = -1;
    char oldName[RWHelper::charLimit];
    for(int i = 0; i < n; ++i) {
        int currentVal;
        EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + 1, currentVal);
        if(currentVal < value && oldValue == -1) {
            EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + 1, value);
            for(int j = 0; j < RWHelper::charLimit; ++j) {
                EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + 2 + j, oldName[j]);
                EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + 2 + j, name[j]);
            }
            oldValue = currentVal;
            continue;
        }
        if(oldValue > 0) { // shift back the other high scores
            EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + 1, oldValue);
            char aux;
            for(int j = 0; j < RWHelper::charLimit; ++j) {
                EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + 2 + j, aux);
                EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + 2 + j, oldName[j]);
                oldName[j] = aux;
            }
            oldValue = currentVal;
        }
    }
}

int RWHelper::readContrast() {
    int retval;
    EEPROM.get(RWHelper::contrastAddr, retval);
    return retval;
}

int RWHelper::readLCDBright() {
    int retval;
    EEPROM.get(RWHelper::LCDbrightAddr, retval);
    return retval;
}

int RWHelper::readLEDBright() {
    int retval;
    EEPROM.get(RWHelper::LEDbrightAddr, retval);
    return retval;
}

int RWHelper::readDiff() {
    int retval;
    EEPROM.get(RWHelper::diffAddr, retval);
    return retval;
}

int RWHelper::readHighNum() {
    int retval;
    EEPROM.get(RWHelper::highScoreNumAddr, retval);
    return retval;
}

int RWHelper::readHigh(int which, char* writeHere) {
    int retval;
    int n;
    EEPROM.get(RWHelper::highScoreNumAddr, n);
    if(which <= 0 || which > n)
        return -1; // invalid addresses
    EEPROM.get(RWHelper::highScoreNumAddr + (which - 1) * (sizeof(int) + RWHelper::charLimit * sizeof(char)) + 1, retval);
    for(byte j = 0; j < RWHelper::charLimit; ++j) {
        EEPROM.get(RWHelper::highScoreNumAddr + (which - 1) * (sizeof(int) + RWHelper::charLimit * sizeof(char)) + 2 + j, writeHere[j]);
    }
    return retval;
}

