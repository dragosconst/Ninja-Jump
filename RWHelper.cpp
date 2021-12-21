#include "Arduino.h"
#include "RWHelper.h"

const byte RWHelper::volAddr = HSNUM_ADDR + sizeof(byte) + HIGH_STORED * (sizeof(int) + CHAR_LIMIT * sizeof(char));
const byte RWHelper::themeAddr = RWHelper::volAddr + sizeof(byte);
const byte RWHelper::soundAddr = RWHelper::themeAddr + sizeof(byte);

void RWHelper::clear() {
    byte highScoreSize = sizeof(int) + CHAR_LIMIT * sizeof(char);
    for(int i = 0; i < 5 ; ++i) {
        EEPROM.put(i * sizeof(byte), 0);
        byte j = 0;
        EEPROM.put(HSNUM_ADDR + sizeof(byte) + i * highScoreSize, 0);
        while(j < CHAR_LIMIT) {
            EEPROM.put(HSNUM_ADDR + sizeof(byte) + i * highScoreSize + sizeof(int) +  j * sizeof(char), 0);
            j++;
        }
    }
}

void RWHelper::writeContrast(byte value) {
    EEPROM.put(CONTRAST_ADDR, value);
}

void RWHelper::writeLCDBright(byte value) {
    EEPROM.put(LCD_ADDR, value);
}

void RWHelper::writeHigh(int value, char* name) {
    byte highScoreSize = sizeof(int) + CHAR_LIMIT * sizeof(char);
    byte n;
    EEPROM.get(HSNUM_ADDR, n);
    int oldValue = -1;
    char oldName[CHAR_LIMIT];
    
    for(int i = 0; i < min(5, n + 1); ++i) {
        int currentVal;
        EEPROM.get(HSNUM_ADDR + i * highScoreSize + sizeof(byte), currentVal);
        if(currentVal <= value && oldValue == -1) {
            EEPROM.put(HSNUM_ADDR + i * highScoreSize + sizeof(byte), value);
            for(int j = 0; j < CHAR_LIMIT; ++j) {
                EEPROM.get(HSNUM_ADDR + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, oldName[j]);
                EEPROM.put(HSNUM_ADDR + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, name[j]);
            }
            oldValue = currentVal;
            continue;
        }
        if(oldValue > 0) { // shift back the other high scores
            EEPROM.put(HSNUM_ADDR + i * highScoreSize + sizeof(byte), oldValue);
            char aux;
            for(int j = 0; j < CHAR_LIMIT; ++j) {
                EEPROM.get(HSNUM_ADDR + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, aux);
                EEPROM.put(HSNUM_ADDR + i * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, oldName[j]);
                oldName[j] = aux;
            }
            oldValue = currentVal;
        }
    }
    if(n + 1 <= 5) {
        RWHelper::writeByte(HSNUM_ADDR, n + 1);
    } 

    // this can only happend when there are less than 5 high scores and this is smaller than all the other high scores,
    // therefore we need to write it at the end
    if(oldValue < 0) {
        EEPROM.put(HSNUM_ADDR + n * highScoreSize + sizeof(byte), value);
        for(int j = 0; j < CHAR_LIMIT; ++j) {
            EEPROM.put(HSNUM_ADDR + n * highScoreSize + sizeof(byte) + sizeof(int) + sizeof(char) * j, name[j]);
        }
    }
}

void RWHelper::writeByte(byte addr, byte val) {
    EEPROM.put(addr, val);
}

int RWHelper::readHigh(int which, char* writeHere) {
    int retval;
    byte n;
    EEPROM.get(HSNUM_ADDR, n);
    if(which <= 0 || which > n)
        return -1; // invalid addresses
    EEPROM.get(HSNUM_ADDR + (which - 1) * (sizeof(int) + CHAR_LIMIT * sizeof(char)) + sizeof(byte), retval);
    for(byte j = 0; j < CHAR_LIMIT; ++j) {
        EEPROM.get(HSNUM_ADDR + (which - 1) * (sizeof(int) + CHAR_LIMIT * sizeof(char)) + sizeof(byte) + sizeof(int) + sizeof(char) * j, writeHere[j]);
    }
    return retval;
}

int RWHelper::getLastHigh() {
    int retval = 0;
    byte n;
    EEPROM.get(HSNUM_ADDR, n);
    if(n < 5)
        return 0;
    EEPROM.get(HSNUM_ADDR + sizeof(byte) + (n - 1) * (sizeof(int) + CHAR_LIMIT * sizeof(char)), retval);
    return retval;
}


byte RWHelper::getVal(byte addr) {
    byte retval;
    EEPROM.get(addr, retval);
    return retval;
}