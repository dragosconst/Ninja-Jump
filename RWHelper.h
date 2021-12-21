#ifndef RWHELPER_S
#define RWHELPER_S

#define HIGH_STORED 5

#define CONTRAST_ADDR 0
#define LCD_ADDR 1 * sizeof(byte)
#define LED_ADDR 2 
#define DIFF_ADDR 3
#define HSNUM_ADDR 4
#define CHAR_LIMIT 3

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
public:
    static const byte volAddr;
    static const byte themeAddr;
    static const byte soundAddr;

    static void clear();
    static void writeContrast(byte value);
    static void writeLCDBright(byte value);
    static void writeHigh(int value, char* name);
    static void writeByte(byte addr, byte val);

    static int readHigh(int which, char* writeHere);
    static int getLastHigh();
    static byte getVal(byte addr);
};

#endif