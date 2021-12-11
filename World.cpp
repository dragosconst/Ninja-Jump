#include "Arduino.h"
#include "World.h"

World::World() {
    // byte worldMap[16][8] = {
    //     {0, 1, 1, 1, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 1, 1, 1, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 1, 1, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 1, 1, 1, 0},
    //     {0, 1, 1, 1, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 1, 1, 1, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0, 0, 0},
    //     {0, 1, 1, 1, 1, 1, 0, 0}
    // };
    byte worldMap[16] = {
        B00000000,
        B00000000,
        B01110000,
        B00000000,
        B00000000,
        B00001110,
        B00000000,
        B01100000,
        B00000000,
        B00001110,
        B01110000,
        B00000000,
        B00001110,
        B00000000,
        B00000000,
        B01111100
    };
    // memcpy(this->map, worldMap, 16*8*sizeof(byte));
    this->map = FakeMatrix(World::numRows, World::numCols / 8);
    for(int i = 0; i < 16; ++i) {
        this->map.setByte(Pos(i, 0), worldMap[i]);
    }
}

World::World(LedControl* lc, Player* player) : lc(lc), player(player) {
    byte worldMap[16] = {
        B00000000,
        B00000000,
        B01110000,
        B00000000,
        B00000000,
        B00001110,
        B00000000,
        B01100000,
        B00000000,
        B00001110,
        B01110000,
        B00000000,
        B00001110,
        B00000000,
        B00000000,
        B01111100
    };
    // memcpy(this->map, worldMap, 16*8*sizeof(byte));
    this->map = FakeMatrix(World::numRows, World::numCols / 8);
    for(int i = 0; i < 16; ++i) {
        this->map.setByte(Pos(i, 0), worldMap[i]);
        if(this->map[Pos(i, 1)].check()) {
            Serial.println("dj nichifor solitaru");
        }
        else {
            Serial.println(worldMap[i] & (1 << (7 - (1 % 8))));
            Serial.println(this->map[Pos(i, 1)].check());
        }
    }
}

void World::drawOnMatrix() {
    int row = 0;
    this->map[Pos(this->player->getY(), this->player->getX())] = 1;

    int dec = 6 - max(0, 6 - player->getY()), incr = 7 - dec;
    if(player->getY() == 15) {
        dec += 1;
        incr -= 1;
    }

    // for(int i = 0; i < 8; ++i) {
    //     // this->lc->setRow(0, i, B11111111);
    //     for(int j = 0; j < 8; ++j) {
    //         this->lc->setLed(0, j, i, (i + j) % 2);
    //     }
    // }

    for(int i = player->getY() - dec; i <= player->getY() + incr; ++i) {
        for(int j = 0; j < 8; ++j) {
            int actual_row = 7 - j;
            int col = row; 
            this->lc->setLed(0, col, actual_row, this->map[Pos(i, j)].check());
        }
        row += 1;
    }
}