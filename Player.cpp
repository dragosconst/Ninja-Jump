#include "Arduino.h"
#include "Player.h"

long Player::lastMoved = 0;
long Player::lastJumped = 0;
long Player::lastMovedJump = 0;
long Player::lastFell = 0;

Player::Player(int lives, int height, int x, int y, World* world) : lives(lives), height(height), x(x), y(y), lx(x), ly(y), world(world), jumping(false)
, passedPlatform(false), maxY(10000) { }

void Player::decreaseHealth() {
    this->lives--;
}

void Player::increaseHeight(int amount) {
    this->height += amount;
}

void Player::move(int xVal, int yVal) {
    if(xVal) {
        if(!this->passedPlatform)
            this->world->map[this->y][this->x] = 0;
        else
            this->passedPlatform = false;
    }
    if(xVal == 1) {
        if(x < 7 && this->world->map[this->y][this->x + 1] != 1)
            this->x += 1;
        this->world->map[this->y][this->x] = 1;
    }
    else if(xVal == -1) {
        if(x > 0 && this->world->map[this->y][this->x - 1] != 1)
            this->x -= 1;
        this->world->map[this->y][this->x] = 1;
    }
}

bool Player::onStableGround() const { if(y == 15) return false; return this->world->map[y + 1][x];}

void Player::fall() {
    if(this->onStableGround())
        return;
    if(this->isJumping())
        return;
    if(!this->passedPlatform)
        this->world->map[this->y][this->x] = 0;
    else
        this->passedPlatform = false;
    this->y += 1;
    if(this->y > 15) {
        this->lives -= 1;
        this->x = this->lx;
        this->y = this->ly;
    }
    this->world->map[this->y][this->x] = 1;
}

void Player::jump() {
    if(this->y == 0)
        return;
    if(!this->passedPlatform)
        this->world->map[this->y][this->x] = 0;
    else
        this->passedPlatform = false;
    this->y -= 1;
    if(this->y < this->maxY) {
        this->maxY = this->y;
        this->height += 10;
    }
    if(this->world->map[this->y][this->x] == 1)
        this->passedPlatform = true;
    this->world->map[this->y][this->x] = 1;
}

// used to reset player after game over screen
// obviously, this needs to be completely rewritten for the final version
void Player::clear(int lives, int height, int x, int y) {
    this->lives = lives;
    this->height = height;
    this->x = x;
    this->y = y;
}