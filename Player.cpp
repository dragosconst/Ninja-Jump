#include "Arduino.h"
#include "Player.h"

Player::Player(int lives, int height, int x, int y) : lives(lives), height(height), x(x), y(y), lx(x), ly(y) { }

void Player::decreaseHealth() {
    this->lives--;
}

void Player::increaseHeight(int amount) {
    this->height += amount;
}