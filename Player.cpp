#include "Arduino.h"
#include "Player.h"

long Player::lastMoved = 0;

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