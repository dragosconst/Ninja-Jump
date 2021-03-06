#ifndef SOUNDS_H
#define SOUNDS_H

#define INGAME_THEME_1 1
#define MENU_THEME 2
#define INGAME_THEME_2 3
#define INGAME_THEME_3 4
#define INGAME_THEME_4 5

#define INGAME_FIRST_THEME 1
#define INGAME_SECOND_THEME 2
#define INGAME_THIRD_THEME 3
#define INGAME_FOURTH_THEME 4

#define JUMP_DUR 200
#define CAN_DUR 100
#define DIS_DUR 400
#define MENU_DUR 100


#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "notes.h"

extern const byte SFXpin;
class SoundsManager {
private:
    static DFRobotDFPlayerMini* player;
    static int8_t ingame_theme;
    static bool inMenu, soundsOn;

public:

    static void playTheme();
    static void changeInGame(int8_t newVal);
    static void changeVolume(int8_t vol);
    static void switchMenuState(bool newState) { SoundsManager::inMenu = newState; SoundsManager::playTheme();}

    static void setPlayer(DFRobotDFPlayerMini* player);
    static void setSounds(bool sounds);
    static void playSound(int freq, int dur);
};

#endif