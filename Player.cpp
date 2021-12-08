#include "Arduino.h"
#include "Player.h"

long Player::lastMoved = 0;
long Player::lastJumped = 0;

Player::Player(int lives, int height, int x, int y, World* world) : lives(lives), height(height), x(x), y(y), lx(x), ly(y), world(world) { }

void Player::decreaseHealth() {
    this->lives--;
}

void Player::increaseHeight(int amount) {
    this->height += amount;
}

void Player::move(int xVal, int yVal) {
    if(xVal) {
        this->world->map[this->y][this->x] = 0;
    }
    if(xVal == 1) {
        if(x < 7)
            this->x += 1;
        this->world->map[this->y][this->x] = 1;
    }
    else if(xVal == -1) {
        if(x > 0)
            this->x -= 1;
        this->world->map[this->y][this->x] = 1;
    }
}

bool Player::onStableGround() const { if(y == 15) return false; return this->world->map[y + 1][x];}

void Player::fall() {
    if(this->onStableGround())
        return;
    this->world->map[this->y][this->x] = 0;
    this->y += 1;
    if(this->y > 15) {
        this->lives--;
        this->x = this->lx;
        this->y = this->ly;
        this->world->map[this->y][this->x] = 1;
    }
    else {
        this->world->map[this->y][this->x] = 1;
    }
}