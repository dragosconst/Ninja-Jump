#include "Arduino.h"
#include "World.h"

World::World() {
    byte worldMap[16][8] = {
        {0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0}
    };
    memcpy(this->map, worldMap, 16*8*sizeof(byte));
}

World::World(LedControl* lc, Player* player) : lc(lc), player(player) {
        byte worldMap[16][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0}
    };
    memcpy(this->map, worldMap, 16*8*sizeof(byte));
}

void World::drawOnMatrix() {
    int row = 0;
    this->map[this->player->getY()][this->player->getX()] = 1;

    int dec = 6 - max(0, 6 - player->getY()), incr = 7 - dec;
    if(player->getY() == 15) {
        dec += 1;
        incr -= 1;
    }

    for(int i = player->getY() - dec; i <= player->getY() + incr; ++i) {
        for(int j = 0; j < 8; ++j) {
            this->lc->setLed(0, j, row, this->map[i][j]);
        }
        row += 1;
    }
}