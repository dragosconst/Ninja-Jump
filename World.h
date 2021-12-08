#ifndef WORLD_H
#define WORLD_H

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>

class Player;

class World {
private:
    friend class Player;
    byte map[16][8];
    LedControl* lc;
    Player* player;
public:
    World();
    World(LedControl* lc, Player* player);
    ~World() {}

    void drawOnMatrix();
};

#endif