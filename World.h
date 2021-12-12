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

    static const byte numRows, numCols;
public:
    World();
    World(LedControl* lc, Player* player);
    World(const World& other);
    World& operator=(const World& other);
    ~World() {}

    void drawOnMatrix();
};

#endif