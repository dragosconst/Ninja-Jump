#include "Arduino.h"
#include "Player.h"

const int Player::moveInterval = 80; // interval at which to move
const int Player::moveIntervalInAir = 140;// interval at which to move in the air
const int Player::maxJump = 500; // maximum amount of time spent jumping
const int Player::jumpInterval = 160;
const int Player::fallInterval = 200;
const int Player::flashInterval = 200;

unsigned long Player::lastMoved = 0;
unsigned long Player::lastJumped = 0;
unsigned long Player::lastMovedJump = 0;
unsigned long Player::lastFell = 0;

Player::Player(int lives, int height) : lives(lives), height(height), jumping(false)
, passedPlatform(false), heightMax(height), fallDistance(0), lastFlash(millis()), flashState(true) { }


void Player::move(int xVal, int yVal) {
    if(xVal == 1) {
        if(x < World::numCols - 1 && !this->world->worldMap[Pos(this->y, this->x + 1)].check()) {
            this->world->scrollRight();
        }
        else {
            Serial.println(!this->world->worldMap[Pos(this->y, this->x + 1)].check());
        }
    }
    else if(xVal == -1) {
        if(x > 0 && !this->world->worldMap[Pos(this->y, this->x - 1)].check()) {
            this->world->scrollLeft();
        }
    }
    if(xVal) {
        if(!this->isJumping() && this->onStableGround()) {
            this->lx = this->x;
            this->ly = this->y;
        }
    }
}

bool Player::onStableGround() const { return this->world->worldMap[Pos(y + 1, x)].check();}

void Player::die() {
    this->fallDistance = 0; 
    this->height += FALL_DISTANCE * 10;
    this->lives -= 1;
    this->world->recenter(Pos(this->ly, this->lx));
    this->world->redrawStructs();
    if(!this->world->worldMap[Pos(this->ly + 1, this->lx)].check()) {
        // was on a moving platform, search for it on a very generous range
        for(int8_t x = this->lx - MOV_RANGE; x <= this->lx + MOV_RANGE; ++x) {
            if(x < 0 || x >= World::numCols)
                continue;
            if(this->world->worldMap[Pos(this->ly + 1, this->lx)].check()) {
                this->lx = x;
                break;
            }
        }
        this->world->recenter(Pos(this->ly, this->lx));
        this->world->redrawStructs();
    }
    this->x = this->lx;
    this->y = this->ly;
}

void Player::fall() {
    if(this->onStableGround()) {
        this->fallDistance = 0;
        this->lx = this->x;
        this->ly = this->y;
        return;
    }
    if(this->isJumping())
        return;
    this->world->scrollDown();
    this->height -= 10;
    this->fallDistance += 1;
    if(this->fallDistance == FALL_DISTANCE) {
        this->die();
    }
}

void Player::jump() {
    if(this->y == 0)
        return;
    this->world->scrollUp();
    this->height += 10;
    this->heightMax = max(this->height, this->heightMax);
    if(this->world->worldMap[Pos(this->y, this->x)].check())
        this->passedPlatform = true;
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

void Player::draw() {
    if(this->isJumping() || this->isFalling()) {
        this->world->worldMap[Pos(this->y, this->x)] = 1;
        return;
    }
    if(millis() - this->lastFlash >= Player::flashInterval) {
        this->flashState = !this->flashState;
        this->lastFlash = millis();
    }

    if(this->flashState) {
        this->world->worldMap[Pos(this->y, this->x)] = 1;
    }
    else {
        this->world->worldMap[Pos(this->y, this->x)] = 0;
    }
}