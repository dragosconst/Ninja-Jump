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
    static void clear(); // !!!!! use this as rarely as possible, since it performs a total of 9 (at the moment) writes, which is a lot for arduino
    static void writeContrast(int value);
    static void writeLCDBright(int value);
    static void writeLEDBright(int value);
    static void writeDiff(int value);
    static void writeHighNum(int value);
    static void writeHigh(int value, char* name);

    static int readContrast();
    static int readLCDBright();
    static int readLEDBright();
    static int readDiff();
    static int readHighNum();
    static int readHigh(int which, char* writeHere);
};

#endif