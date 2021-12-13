#ifndef RWHELPER_S
#define RWHELPER_S

#include "Arduino.h"
#include <EEPROM.h>

/**
 * @brief 
 * This class will serve to perform R\W operations on the EEPROM, instead of doing them directly in code.
 * This way, I don't have to worry about hardcoding where I store my stuff elsewhere and I can also do some checks to minimize writing, without
 * getting more messy code elsewhere.
 * 
 * Highscores are stored like a (score, name) tuple, where score is a byte and name occupies charLimit bytes
 */
class RWHelper {
private:
    static const byte contrastAddr;
    static const byte LCDbrightAddr;
    static const byte LEDbrightAddr;
    static const byte diffAddr;
    static const byte highScoreNumAddr; // from 4 onwards the highscores will be stored
    static const byte charLimit; // limit of chars in a name
public:
    static void clear();
    static void writeContrast(byte value);
    static void writeLCDBright(byte value);
    static void writeLEDBright(byte value);
    static void writeDiff(byte value);
    static void writeHighNum(byte value);
    static void writeHigh(int value, char* name);

    static byte readContrast();
    static byte readLCDBright();
    static byte readLEDBright();
    static byte readDiff();
    static byte readHighNum();
    static int readHigh(int which, char* writeHere);
    static int getLastHigh();
};

#endif