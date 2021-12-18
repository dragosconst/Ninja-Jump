#include "Arduino.h"
#include "Player.h"

const int Player::moveInterval = 80; // interval at which to move
const int Player::moveIntervalInAir = 140;// interval at which to move in the air
const int Player::maxJump = 500; // maximum amount of time spent jumping
const int Player::jumpInterval = 160;
const int Player::fallInterval = 200;

unsigned long Player::lastMoved = 0;
unsigned long Player::lastJumped = 0;
unsigned long Player::lastMovedJump = 0;
unsigned long Player::lastFell = 0;

Player::Player(int lives, int height) : lives(lives), height(height), jumping(false)
, passedPlatform(false), maxY(10000), heightMax(height), fallDistance(0) { }


void Player::decreaseHealth() {
    this->lives--;
}

void Player::increaseHeight(int amount) {
    this->height += amount;
}

void Player::move(int xVal, int yVal) {
    if(xVal == 1) {
        if(x < World::numCols - 1 && !this->world->worldMap[Pos(this->y, this->x + 1)].check()) {
            // this->x += 1;
            this->world->scrollRight();
            // Serial.println("moving to the right");
        }
        else {
            Serial.println(!this->world->worldMap[Pos(this->y, this->x + 1)].check());
        }
        this->world->worldMap[Pos(this->y, this->x)] = 1;
    }
    else if(xVal == -1) {
        if(x > 0 && !this->world->worldMap[Pos(this->y, this->x - 1)].check()) {
            // this->x -= 1;
            this->world->scrollLeft();
            // Serial.println("moving to the left");
        }
        else {
            // Serial.println(!this->world->worldMap[Pos(this->y, this->x - 1)].check());
        }
        this->world->worldMap[Pos(this->y, this->x)] = 1;
    }
    if(xVal) {
        if(!this->isJumping() && this->onStableGround()) {
            this->lx = this->x;
            this->ly = this->y;
        }
    }
}

// This will check if the player could reach (x, y) by jumping from (sx, sy)
// It is assumed that y <= sy
bool Player::isInRange(byte x,  byte y, byte sx, byte sy) {
    if(y > sy - Player::maxJump / Player::jumpInterval)
        return false;
    // the furthest it can continously move on the x axis in one jump
    int posx =  Player::maxJump / Player::moveIntervalInAir;
    if(x >= sx) {
        return sx + posx >= x;
    }
    else {
        return sx - posx <= x;
    }
}

bool Player::onStableGround() const { return this->world->worldMap[Pos(y + 1, x)].check();}

void Player::fall() {
    if(this->onStableGround()) {
        this->fallDistance = 0;
        return;
    }
    if(this->isJumping())
        return;
    this->world->scrollDown();
    this->height -= 10;
    this->fallDistance += 1;
    // this->y += 1;
    if(this->fallDistance == FALL_DISTANCE) {
        this->lives -= 1;
        this->fallDistance = 0;
        this->height += FALL_DISTANCE * 10;
        this->world->recenter(Pos(this->ly, this->lx));
        this->x = this->lx;
        this->y = this->ly;
    }
    this->world->worldMap[Pos(this->y, this->x)] = 1;
}

void Player::jump() {
    if(this->y == 0)
        return;
    this->world->scrollUp();
    this->height += 10;
    this->heightMax = max(this->height, this->heightMax);
    // this->y -= 1;
    if(this->world->worldMap[Pos(this->y, this->x)].check())
        this->passedPlatform = true;
    this->world->worldMap[Pos(this->y, this->x)] = 1;
}

// used to reset player after game over screen
// obviously, this needs to be completely rewritten for the final version
void Player::clear(int lives, int height, int x, int y) {
    this->lives = lives;
    this->height = height;
    this->heightMax = height;
    this->x = x;
    this->y = y;
}