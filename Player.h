#ifndef PLAYER_H
#define PLAYER_H

#define FALL_DISTANCE 10

#include "Arduino.h"
#include "World.h"
#include "FakeMatrix.h"

class World;


class Player {
private:
    int x, y;
    int lx, ly;
    int maxY;
    World* world;
    bool jumping;
    bool passedPlatform; // for restoring platform integrity after jumping through them
    int lives;
    int heightMax, height;
    byte fallDistance = 0;
public:
    static const int moveInterval; // interval at which to move
    static const int moveIntervalInAir;
    static unsigned long lastMoved;
    static const int maxJump; // maximum amount of time spent jumping
    static const int jumpInterval; // interval at which to move in the air
    static const int fallInterval;
    static unsigned long lastJumped;
    static unsigned long lastMovedJump;
    static unsigned long lastFell;

    Player() {lives = 0; height = 0;}
    Player(int lives, int height, int x, int y, World* world);
    ~Player() {}

    void decreaseHealth();
    void increaseHeight(int amount);

    void move(int xVal, int yVal);
    bool onStableGround() const;
    void fall();
    void jump();
    void startJumping() {Player::lastJumped = millis(); Player::lastMoved = 0; this->jumping = true;}
    void stopJumping() { this->jumping = false;}
    void clear(int lives, int height, int x, int y);
    static bool isInRange(byte x, byte y, byte sx, byte sy);
    void setPosition(Pos pos) { this->x = pos.j; this->y = pos.i;}
    void setWorld(World* world) { this->world = world;}
    void setPassedPlatform(bool val) { this->passedPlatform = val;}
    void setLastPos(Pos pos) { this->lx = pos.j; this->ly = pos.i;}

    int getX() const { return this->x; }
    int getY() const { return this->y; }
    Pos getPos() const { return Pos(this->y, this->x);}
    Pos getLastPos() const { return Pos(this->ly, this->lx);}
    bool isJumping() const { return this->jumping; }
    bool isFalling() const { return !this->onStableGround() && !this->jumping;}
    bool jumpedThroughPlatform() const { return this->passedPlatform;}
    int getLives() const { return this->lives;}
    int getHeight() const { return this->height;}
    static int getYRange() { return Player::maxJump / Player::jumpInterval;}
    static int getXRange() { return Player::maxJump/Player::moveIntervalInAir;}
    int* getLivesAddr()  { return &this->lives;}
    int* getHeightAddr() { return &this->heightMax; }
};

#endif