#include "Arduino.h"
#include "RWHelper.h"

const byte RWHelper::contrastAddr = 0;
const byte RWHelper::LCDbrightAddr = 1 * sizeof(byte);
const byte RWHelper::LEDbrightAddr = 2 * sizeof(byte);
const byte RWHelper::diffAddr = 3 * sizeof(byte);
const byte RWHelper::highScoreNumAddr = 4 * sizeof(byte); // from 4 onwards the highscores will be stored
const byte RWHelper::charLimit = 3;

void RWHelper::clear() {
    byte highScoreSize = sizeof(int) + RWHelper::charLimit * sizeof(char);
    for(int i = 0; i < 5 ; ++i) {
        EEPROM.put(i * sizeof(byte), 0);
        byte j = 0;
        EEPROM.put(RWHelper::highScoreNumAddr + sizeof(byte) + i * highScoreSize, 0);
        while(j < RWHelper::charLimit) {
            EEPROM.put(RWHelper::highScoreNumAddr + sizeof(byte) + i * highScoreSize + sizeof(int) +  j * sizeof(char), 0);
            j++;
        }
    }
}

void RWHelper::writeContrast(byte value) {
    EEPROM.put(RWHelper::contrastAddr, value);
}

void RWHelper::writeLCDBright(byte value) {
    EEPROM.put(RWHelper::LCDbrightAddr, value);
}

void RWHelper::writeLEDBright(byte value) {
    EEPROM.put(RWHelper::LEDbrightAddr, value);
}

void RWHelper::writeDiff(byte value) {
    EEPROM.put(RWHelper::diffAddr, value);
}

void RWHelper::writeHighNum(byte value) {
    EEPROM.put(RWHelper::highScoreNumAddr, value);
}

void RWHelper::writeHigh(int value, char* name) {
    byte highScoreSize = sizeof(int) + RWHelper::charLimit * sizeof(char);
    byte n;
    EEPROM.get(RWHelper::highScoreNumAddr, n);
    int oldValue = -1;
    char oldName[RWHelper::charLimit];
    
    for(int i = 0; i < min(5, n + 1); ++i) {
        int currentVal;
        EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte), currentVal);
        if(currentVal <= value && oldValue == -1) {
            EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte), value);
            for(int j = 0; j < RWHelper::charLimit; ++j) {
                EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, oldName[j]);
                EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, name[j]);
            }
            oldValue = currentVal;
            continue;
        }
        if(oldValue > 0) { // shift back the other high scores
            EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte), oldValue);
            char aux;
            for(int j = 0; j < RWHelper::charLimit; ++j) {
                EEPROM.get(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, aux);
                EEPROM.put(RWHelper::highScoreNumAddr + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, oldName[j]);
                oldName[j] = aux;
            }
            oldValue = currentVal;
        }
    }
    if(n + 1 <= 5) {
        RWHelper::writeHighNum(n + 1);
    } 

    // this can only happend when there are less than 5 high scores and this is smaller than all the other high scores,
    // therefore we need to write it at the end
    if(oldValue < 0) {
        EEPROM.put(RWHelper::highScoreNumAddr + n * highScoreSize + sizeof(byte), value);
        for(int j = 0; j < RWHelper::charLimit; ++j) {
            EEPROM.put(RWHelper::highScoreNumAddr + n * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, name[j]);
        }
    }
}

byte RWHelper::readContrast() {
    byte retval;
    EEPROM.get(RWHelper::contrastAddr, retval);
    return retval;
}

byte RWHelper::readLCDBright() {
    byte retval;
    EEPROM.get(RWHelper::LCDbrightAddr, retval);
    return retval;
}

byte RWHelper::readLEDBright() {
    byte retval;
    EEPROM.get(RWHelper::LEDbrightAddr, retval);
    return retval;
}

byte RWHelper::readDiff() {
    byte retval;
    EEPROM.get(RWHelper::diffAddr, retval);
    return retval;
}

byte RWHelper::readHighNum() {
    byte retval;
    EEPROM.get(RWHelper::highScoreNumAddr, retval);
    return retval;
}

int RWHelper::readHigh(int which, char* writeHere) {
    int retval;
    byte n;
    EEPROM.get(RWHelper::highScoreNumAddr, n);
    if(which <= 0 || which > n)
        return -1; // invalid addresses
    EEPROM.get(RWHelper::highScoreNumAddr + (which - 1) * (sizeof(int) + RWHelper::charLimit * sizeof(char)) + sizeof(byte), retval);
    for(byte j = 0; j < RWHelper::charLimit; ++j) {
        EEPROM.get(RWHelper::highScoreNumAddr + (which - 1) * (sizeof(int) + RWHelper::charLimit * sizeof(char)) + sizeof(byte) + sizeof(int) + sizeof(char) * j, writeHere[j]);
    }
    return retval;
}

int RWHelper::getLastHigh() {
    int retval = 0;
    byte n;
    EEPROM.get(RWHelper::highScoreNumAddr, n);
    if(n < 5)
        return 0;
    EEPROM.get(RWHelper::highScoreNumAddr + sizeof(byte) + (n - 1) * (sizeof(int) + RWHelper::charLimit * sizeof(char)), retval);
    return retval;
}
