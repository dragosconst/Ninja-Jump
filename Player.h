#ifndef PLAYER_H
#define PLAYER_H

#include "Arduino.h"

class Player {
private:
    int x, y;
    int lx, ly;
    int lives;
    int height;
public:
    Player() {lives = 0; height = 0;}
    Player(int lives, int height, int x, int y);
    ~Player() {}

    void decreaseHealth();
    void increaseHeight(int amount);

    int* getLivesAddr()  { return &this->lives;}
    int* getHeightAddr() { return &this->height; }
};

#endif