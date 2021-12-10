#include "Arduino.h"
#include "RWHelper.h"

void RWHelper::clear() {
    for(int i = 0; i < 10; ++i) {
        EEPROM.update(i, 0);
    }
}

void RWHelper::writeContrast(int value) {
    EEPROM.update(RWHelper::contrastAddr, value);
}

void RWHelper::writeLCDBright(int value) {
    EEPROM.update(RWHelper::LCDbrightAddr, value);
}

void RWHelper::writeLEDBright(int value) {
    EEPROM.update(RWHelper::LEDbrightAddr, value);
}

void RWHelper::writeDiff(int value) {
    EEPROM.update(RWHelper::diffAddr, value);
}

void RWHelper::writeHighNum(int value) {
    EEPROM.update(RWHelper::highScoreNumAddr, value);
}

void RWHelper::writeHigh(int value) {
    int n = EEPROM.read(RWHelper::highScoreNumAddr);
    int oldValue = -1;
    for(int i = 1; i <= n; ++i) {
        int currentVal = EEPROM.read(RWHelper::highScoreNumAddr + i);
        if(currentVal < value) {
            EEPROM.update(RWHelper::highScoreNumAddr + i, value);
            oldValue = currentVal;
        }
        if(oldValue > 0) { // shift back the other high scores
            EEPROM.update(RWHelper::highScoreNumAddr + i, oldValue);
            oldValue = currentVal;
        }
    }
}

int RWHelper::readContrast() {
    return EEPROM.read(RWHelper::contrastAddr);
}

int RWHelper::readLCDBright() {
    return EEPROM.read(RWHelper::LCDbrightAddr);
}

int RWHelper::readLEDBright() {
    return EEPROM.read(RWHelper::LEDbrightAddr);
}

int RWHelper::readDiff() {
    return EEPROM.read(RWHelper::diffAddr);
}

int RWHelper::readHighNum() {
    return EEPROM.read(RWHelper::highScoreNumAddr);
}

int RWHelper::readHigh(int which) {
    int n = EEPROM.read(RWHelper::highScoreNumAddr);
    if(which <= 0 || which > n)
        return -1; // invalid addresses
    return EEPROM.read(RWHelper::highScoreNumAddr + which);
}

