#ifndef PLAYER_H
#define PLAYER_H

#define FALL_DISTANCE 10
#define MAX_LIVES 2
#define START_HEIGHT 10

#define RONIN 250
#define APPRENTICE 500
#define NINJA 1000
#define SAMURAI 1500
#define ASSASSIN 2200
#define MONK 2700
#define SHOGUN 3300
#define SEKIRO 4000


#include "Arduino.h"
#include "World.h"
#include "FakeMatrix.h"
#include "SoundsManager.h"

class World;


class Player {
private:
    int x, y;
    int lx, ly;
    World* world;
    bool jumping;
    bool passedPlatform; // for restoring platform integrity after jumping through them
    int lives;
    int heightMax, height;
    byte fallDistance = 0;
    bool flashState;
    long lastFlash;
public:
    static const int moveInterval; // interval at which to move
    static const int moveIntervalInAir;
    static unsigned long lastMoved;
    static const int maxJump; // maximum amount of time spent jumping
    static const int jumpInterval; // interval at which to move in the air
    static const int fallInterval;
    static const int flashInterval;
    static unsigned long lastJumped;
    static unsigned long lastMovedJump;
    static unsigned long lastFell;

    Player() {lives = 0; height = 0;}
    Player(int lives, int height);
    ~Player() {}

    void move(int xVal, int yVal);
    bool onStableGround() const;
    void fall();
    void jump();
    void startJumping() {Player::lastJumped = millis(); Player::lastMoved = 0; this->jumping = true; this->fallDistance = 0; SoundsManager::playSound(NOTE_FS3, JUMP_DUR);}
    void stopJumping() { this->jumping = false;}
    void clear(int lives, int height, int x, int y);
    void setPosition(Pos pos) { this->x = pos.j; this->y = pos.i; this->setLastPos(pos);}
    void setWorld(World* world) { this->world = world;}
    void setPassedPlatform(bool val) { this->passedPlatform = val;}
    void setLastPos(Pos pos) { this->lx = pos.j; this->ly = pos.i;}
    void die();
    void draw();

    int getX() const { return this->x; }
    int getY() const { return this->y; }
    Pos getPos() const { return Pos(this->y, this->x);}
    Pos getLastPos() const { return Pos(this->ly, this->lx);}
    bool isJumping() const { return this->jumping; }
    bool isFalling() const { return !this->onStableGround() && !this->jumping;}
    bool jumpedThroughPlatform() const { return this->passedPlatform;}
    int getLives() const { return this->lives;}
    int getHeight() const { return this->heightMax;}
    static int getYRange() { return Player::maxJump / Player::jumpInterval;}
    static int getXRange() { return Player::maxJump/Player::moveIntervalInAir;}
    int* getLivesAddr()  { return &this->lives;}
    int* getHeightAddr() { return &this->heightMax; }
    World* getWorld() { return this->world;}
};

#endif