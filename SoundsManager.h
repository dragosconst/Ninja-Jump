#ifndef SOUNDS_H
#define SOUNDS_H

#define INGAME_THEME_1 1
#define MENU_THEME 2
#define INGAME_THEME_2 3

#define INGAME_FIRST_THEME 1
#define INGAME_SECOND_THEME 2

#define JUMP_DUR 500
#define CAN_DUR 1000
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
    static void playJump();
    static void playCanons();
    static void playDisappearing();
    static void playMenuScroll();
    static void playMenuClick();
};

#endif