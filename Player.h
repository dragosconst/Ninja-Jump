#ifndef PLAYER_H
#define PLAYER_H

#include "Arduino.h"
#include "World.h"

class World;

class Player {
private:
    int x, y;
    int lx, ly;
    int lives;
    int height;
    World* world;
public:
    static const int moveInterval = 40; // interval at which to move
    static long lastMoved;
    static const int maxJump = 200; // maximum amount of time spent jumping

    Player() {lives = 0; height = 0;}
    Player(int lives, int height, int x, int y, World* world);
    ~Player() {}

    void decreaseHealth();
    void increaseHeight(int amount);

    void move(int xVal, int yVal);

    int getX() const { return this->x; }
    int getY() const { return this->y; }
    int* getLivesAddr()  { return &this->lives;}
    int* getHeightAddr() { return &this->height; }
};

#endif