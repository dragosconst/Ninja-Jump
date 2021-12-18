#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "Arduino.h"
#include "Menu.h"
#include "RWHelper.h"

Menu* createCongratulationsMenu();
Menu* createGameOverMenu();
Menu* createDisplayMenu();

class StateMachine {
private:
    GameStates state;
    Menu** currentMenu;
    Player** player;
    World** world;
    LedControl* lc;
    int* diff;
public:
    StateMachine() {}
    StateMachine(Menu** currentMenu, Player** player, World** world, LedControl* lc, int* diff);
    ~StateMachine() {}

    void updateState();

    GameStates getState() const { return this->state; } 
};

#endif