#ifndef WORLD_H
#define WORLD_H

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>
#include "FakeMatrix.h"

class Player;

class World {
private:
    friend class Player;
    FakeMatrix map;
    LedControl* lc;
    Player* player;

    static const byte numRows = 16, numCols = 8;
public:
    World();
    World(LedControl* lc, Player* player);
    ~World() {}

    void drawOnMatrix();
};

#endif