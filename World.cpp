#include "Arduino.h"
#include "World.h"

const byte World::numRows = 24;
const byte World::numCols = 24;

World::World() {
    // // byte worldMap[16][8] = {
    // //     {0, 1, 1, 1, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 1, 1, 1, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 1, 1, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 1, 1, 1, 0},
    // //     {0, 1, 1, 1, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 1, 1, 1, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 0, 0, 0, 0, 0, 0, 0},
    // //     {0, 1, 1, 1, 1, 1, 0, 0}
    // // };
    // byte worldMap[16] = {
    //     B00000000,
    //     B00000000,
    //     B01110000,
    //     B00000000,
    //     B00000000,
    //     B00001110,
    //     B00000000,
    //     B01100000,
    //     B00000000,
    //     B00001110,
    //     B01110000,
    //     B00000000,
    //     B00001110,
    //     B00000000,
    //     B00000000,
    //     B01111100
    // };
    // // memcpy(this->worldMap, worldMap, 16*8*sizeof(byte));
    // this->worldMap = FakeMatrix(World::numRows, World::numCols / 8);
    // for(int i = 0; i < 16; ++i) {
    //     this->worldMap.setByte(Pos(i, 0), worldMap[i]);
    // }
}

World::World(LedControl* lc, Player* player, byte difficulty) : lc(lc), player(player), difficulty(difficulty),
difficultyStepY(Player::getYRange() / 3), difficultyStepX(Player::getXRange() / 3) {
    // byte worldMap[16] = {
    //     B00000000,
    //     B00000000,
    //     B01110000,
    //     B00000000,
    //     B00000000,
    //     B00001110,
    //     B00000000,
    //     B01100000,
    //     B00000000,
    //     B00001110,
    //     B01110000,
    //     B00000000,
    //     B00001110,
    //     B00000000,
    //     B00000000,
    //     B01111100
    // };
    // // memcpy(this->worldMap, worldMap, 16*8*sizeof(byte));
    this->worldMap = FakeMatrix(World::numRows, World::numCols / 8);
    this->roomsState = FakeMatrix(World::numRows / 8, 1);
    this->emptyColumnsLeft = this->emptyColumnsRight = this->emptyLinesDown = this->emptyLinesUp = 0;
    this->generateFromLast(true);
    this->generateFromLast();
    this->generateFromLast();
    Serial.println(this->difficultyStepX);
    Serial.println(this->difficultyStepY);
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
        Serial.print(this->last.x); Serial.print(" "); Serial.print(this->last.y); Serial.println();
    // for(int i = 0; i < 16; ++i) {
    //     this->worldMap.setByte(Pos(i, 0), worldMap[i]);
    //     if(this->worldMap[Pos(i, 1)].check()) {
    //     }
    //     else {
    //     }
    // }
}

World& World::operator=(const World& other) {
    this->lc = other.lc;
    this->player = other.player;
    this->worldMap = other.worldMap;
    this->difficultyStepX = other.difficultyStepX;
    this->difficultyStepY = other.difficultyStepY;
    this->difficulty = other.difficulty;
    this->last = other.last;
    this->secondLast = other.secondLast;
    return *this;
}
World::World(const World& other) {
    this->lc = other.lc;
    this->player = other.player;
    this->worldMap = other.worldMap;
    this->difficultyStepX = other.difficultyStepX;
    this->difficultyStepY = other.difficultyStepY;
    this->difficulty = other.difficulty;
    this->last = other.last;
    this->secondLast = other.secondLast;
}

void World::scrollUp() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform();
    for(int i = World::numRows - 2; i >= 0; --i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i + 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    this->last.y += 1;
    this->secondLast.y += 1;
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(0, j), B00000000);
    }
    this->emptyLinesUp += 1; 
    this->checkCamera();
}

void World::scrollDown() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform();
    this->last.y -= 1;
    this->secondLast.y -= 1;
    for(int i = 1; i < World:: numRows; ++i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i - 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(World::numRows - 1, j), B00000000);
    }
    this->emptyLinesDown += 1; 
    this->checkCamera();
}

void World::scrollLeft() { 
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform();
    this->last.x += 1;
    this->secondLast.x += 1;
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = World::numCols - 2; j >= 0; --j) {
            this->worldMap[Pos(i, j + 1)] = this->worldMap[Pos(i, j)];
        }
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, 0)] = 0;
    }
    this->emptyColumnsLeft += 1;
    this->checkCamera();
}

void World::scrollRight() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform();
    this->last.x -= 1;
    this->secondLast.x -= 1;
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 1; j < World::numCols; ++j) {
            this->worldMap[Pos(i, j - 1)] = this->worldMap[Pos(i, j)];
        }
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, World::numCols - 1)] = 0;
    }
    this->emptyColumnsRight += 1;
    this->checkCamera();
}

// check if the last structure is now in the panning camera, in which case generate a new last structure
void World::checkCamera() {
    if(this->last.x >= 8 && this->last.x < 16 && this->last.y >= 8 && this->last.y < 16) {
        Serial.println("huh");
        Serial.print(this->last.x); Serial.print(" "); Serial.print(this->last.y); Serial.println();
        this->generateFromLast();
    }
}

Structure World::generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor) {
    // int8_t lastContJ = -1, firstContJ = -1; // last and first places it can stretch to
    // for(int j = anchor; j < jend; ++j) {
    //     if(this->worldMap[Pos(i, j)].check()) {
    //         lastContJ = j - 1;
    //         break;
    //     }
    // }
    // if(lastContJ == -1) {
    //     lastContJ = jend - 1;
    // }

    // for(int j = anchor; j >= jst; --j) {
    //     if(this->worldMap[Pos(i, j)].check()) {
    //         firstContJ = j + 1;
    //     }
    // }
    // if(firstContJ == -1) {
    //     firstContJ = jst;
    // }
    //  Serial.println("----------------------------------------");
    //         for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    int8_t where = random(jst, jend - 1);
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        this->worldMap[Pos(i, j)] = 1;
    }
    // Serial.println("line structure is between");
    // Serial.print(min(where, anchor)); Serial.print(" "); Serial.print(max(where, anchor)); Serial.println();
    // Serial.println("at line");
    // Serial.println(i);
    // delay(10);    for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    return Structure(min(where, anchor), i, max(where, anchor) - min(where, anchor), 0);
}

// pointy lines look something like:
//*___*
//_***_
Structure World::generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor) {
    //  Serial.println("----------------------------------------");
    //         for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    int8_t where = random(jst, jend - 1); 
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        if(j == min(where, anchor) || j == max(where, anchor)) {
            this->worldMap[Pos(i - 1, j)] = 1;
        }
        else {
            this->worldMap[Pos(i, j)] = 1;
        }
    }
    // Serial.println("pointy structure is between");
    // Serial.print(min(where, anchor)); Serial.print(" "); Serial.print(max(where, anchor)); Serial.println();
    // Serial.println("at line");
    // Serial.println(i - 1);
    // delay(10);
    //     for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    return Structure(min(where, anchor), i - 1, max(where, anchor) - min(where, anchor), 1);
}

// generate a structure in given params
Structure World::generateStructure(int8_t x_first, int8_t y_first, int8_t x_last, int8_t y_last, int8_t xMax, int8_t yMax) {
    byte x = random(x_first, x_last - 1), y = random(y_first, y_last - 1); // anchor point of structure
    Serial.print("X_first is "); Serial.print(x_first); Serial.print(" x_last is "); Serial.print(x_last); Serial.print(" y_first is "); Serial.print(y_first); Serial.print(" y_last is "); Serial.println(y_last);
    Serial.print("y is "); Serial.println(y);
    if(y == World::numRows - 1) {
        return this->generateLine(y, x_first, xMax, x);
    }
    else {
        byte dice = random(2);
        if(dice == GEN_LINE) {
           return this->generateLine(y, x_first, xMax, x);
        }
        else if(dice == GEN_POINTY) {
            return this->generatePointyLine(y, x_first, xMax, x);
        }
    }

}

// check if this position would be too close to other structures
bool World::tooClose(int8_t i, int8_t j) {
    for(int y = World::numRows - 1; y >= 0; --y) {
        for(int x = 0; x < World::numCols; ++x) {
            if(this->worldMap[Pos(y, x)].check() && !(player->getX() == x && player->getY() == y)) {
                if(y >= i && y - this->difficultyStepY * (this->difficulty + 1) < i - 1 && x <= j && x + this->difficultyStepX * (this->difficulty + 1) > j)
                {
                    Serial.println(this->worldMap[Pos(y, x)].check());
                    Serial.print(i); Serial.print(" ");Serial.print(j); Serial.print(" "); Serial.print(y); Serial.print(" "); Serial.print(x); Serial.println();
                    return true;
                }
                // if(y >= i && y - this->difficultyStepY * (this->difficulty + 1) < i && x >= j && x + this->difficultyStepX * (this->difficulty + 1) < j) {
                //     return true;
                // }
            }
        }
    }
    return false;
}

// get the best jumping range that includes i and j
Structure World::getBestRange(byte i, byte j) {
    Pos closest(1000, 0);
    for(int y = World::numRows - 1; y >= 0; --y) {
        for(int x = 0; x < World::numCols; ++x) {
            if(this->worldMap[Pos(y, x)].check() && !(player->getX() == x && player->getY() == y)) {
                if(x <= j && x + Player::maxJump / Player::moveIntervalInAir >= j) {
                    closest.j = max(x, closest.j);
                }
                else if(x > j && x - Player::maxJump / Player::moveIntervalInAir <= j) {
                    closest.j = max(x, closest.j);
                }

                if(y <= j && y - Player::maxJump / Player::jumpInterval <= j) {
                    closest.i = min(y, closest.i);
                } 
            }
        }
    }
}

// get range from highest point to the left
Structure World::getBestRange(Structure structure) {
    int8_t topy = 100, leftx = 100, height, width;
    for(int8_t y = structure.y; y <= structure.y + structure.height; ++y) {
        for(int8_t x = structure.x; x <= structure.x + structure.width; ++x) {
            if(this->worldMap[Pos(y, x)].check()) {

                if(topy > max(y - Player::maxJump / Player::jumpInterval + 1, 0)) {
                    topy = max(y - Player::maxJump / Player::jumpInterval + 1, 0);
                    height = y - topy;
                    leftx = max(x - Player::maxJump / Player::moveIntervalInAir, 0);
                    width = min(2 * Player::maxJump / Player::moveIntervalInAir, World::numCols - 1 - leftx);
                }
            }
        }
    }
    return Structure(leftx, topy, width, height);

}

Structure World::getMinDiff(Structure structure) {
    int8_t topy = 100, leftx = 100, height, width;
    for(int8_t y = structure.y; y <= structure.y + structure.height; ++y) {
        for(int8_t x = structure.x; x <= structure.x + structure.width; ++x) {
            if(this->worldMap[Pos(y, x)].check() && !(player->getX() == x && player->getY() == y)) {
                if(topy > max(y - this->difficultyStepY * this->difficulty, 0)) {
                    topy = max(y - this->difficultyStepY * this->difficulty, 0);
                    height = y - topy;
                    leftx = max(x -this->difficultyStepX * this->difficulty, 0);
                    width = min(2 * this->difficultyStepX * this->difficulty, World::numCols - 1 - leftx);
                }                
            }
        }
    }    
    return Structure(leftx, topy, width, height);
}

// get total jump range from a point
Structure World::getTotalRange(int8_t y, int8_t x) {
    int8_t left = max((x - Player::maxJump / Player::moveIntervalInAir), 0);
    int8_t right = min(x + Player::maxJump / Player::moveIntervalInAir, World::numCols - 1);
    int8_t up = max(y - Player::maxJump / Player::jumpInterval, 0);
    return Structure(left, up, right - left, y - up);
}

// get area which is reachable from last generated structure, but not from second last structure
// it's going to be a rought estimate, since we need a square
Structure World::withoutIntersection(Structure s1, Structure s2) {
    if(s1.x <= s2.x) {
        // look to the left
        int8_t x = s1.x, y = s1.y, w = 0, hw = 0, wh = 0, h = 0;
        /**
         * @brief 
         * Since the other area of action is a square too, to find the optimal range without intersection we need to first try to make
         * the width as big as possible, and try to make a rect with that, and subsequently do the same thing with the height. In the end,
         * we'll be left with the biggest possible rectangle.
         */
        if(s1.y < s2.y) {
            w = s1.width;
            wh = min(s2.y - s1.y - 1, s1.height);
        }
        else {
            w = min(s2.x - s1.x - 1, s1.width);
            wh = s1.height;
        }

        if(s1.x < s2.x) {
            h = s1.height;
            hw = min(s2.x - s1.x - 1, s1.width);
        }
        else {
            h = min(s2.y - s1.y - 1, s1.height);
            hw = s1.width;
        }

        if(w * wh >= h * hw) {
            return Structure(x, y, w, wh);
        }
        return Structure(x, y, hw, h);
    }
    else {
        // look to the right
        int8_t x = s1.x + s1.width, y = s1.y, w = 0, hw = 0, wh = 0, h = 0;
        if(s1.y < s2.y) {
            w = s1.width;
            wh = min(s2.y - s1.y - 1, s1.height);
        }
        else {
            w = min(s2.x - s1.x - 1, s1.width);
            wh = s1.height;
        }

        if(s1.x < s2.x) {
            h = s1.height;
            hw = min(s2.x - s1.x - 1, s1.width);
        }
        else {
            h = min(s2.y - s1.y - 1, s1.height);
            hw = s1.width;
        }

        if(w * wh >= h * hw) {
            return Structure(x - w, y, w, wh);
        }
        return Structure(x - hw, y, hw, h);
    }
}

void World::generateFromLast(bool first = false) {
    if(first) {
        this->secondLast = Structure(-100, -100, -100, -100);
        this->last = this->generateLine(13, 8, 16, 12);
        this->player->setPosition(Pos(12, 12));
    }
    else {
        if(this->secondLast.x == -100) {
            // no second last yet, can generate anywhere within range for the first structure
            this->secondLast = this->last;
            // Serial.print("last is: x = "); Serial.print(last.x); Serial.print(" y = "); Serial.print(last.y); Serial.print(" height = "); Serial.print(last.height); Serial.print(" width = "); Serial.println(last.width);
            Structure range = this->getBestRange(this->last);
            range = this->withoutIntersection(range, this->getMinDiff(this->last));
            // Serial.print("range is: x = "); Serial.print(range.x); Serial.print(" y = "); Serial.print(range.y); Serial.print(" height = "); Serial.print(range.height); Serial.print(" width = "); Serial.println(range.width);
            this->last = this->generateStructure(range.x, range.y, min(range.x + range.width + 1, World::numCols - 1), 
            max(range.y - range.height - 1, 0), min(range.x + range.width + 1 + 8, World::numCols - 1), max(range.y - range.height - 1 - 8, 0));
        }
        else {
            Structure rangeLast = this->getBestRange(this->last);
            Structure rangeSecondLast = this->getBestRange(this->secondLast);
            Structure range = this->withoutIntersection(rangeLast, rangeSecondLast);                        
            Serial.print("range is: x = "); Serial.print(range.x); Serial.print(" y = "); Serial.print(range.y); Serial.print(" height = "); Serial.print(range.height); Serial.print(" width = "); Serial.println(range.width);
            range = this->withoutIntersection(range, this->getMinDiff(this->last));
            this->secondLast = this->last;
            this->last = this->generateStructure(range.x, range.y, min(range.x + range.width + 1, World::numCols - 1), 
            max(range.y - range.height - 1, 0), min(range.x + range.width + 1 + 8, World::numCols - 1), max(range.y - range.height - 1 - 8, 0));
        }
    }
}

/**
 * @brief 
 * This method will handle most procedural generation. When filling a room, multiple things will be considered, like difficulty, whether
 * adjacent rooms are empty or not (to generate more complex structures), what power-ups the player has etc.
 * For starters, I will generate either regular platforms or pointy platforms. TODO: add padogas
 * Difficulty: it will influence multiple things, but for starters, how far apart platforms will be. A difficulty of 0 will have at minimum
 * a separation of player.range / 3, diff of 1 will be 2 * player.range / 3 and diff of 2 will be exactly how far the player can reach on the Oy
 * axis. On the Ox axis, I will do the same for now.
 * The way it should work is:
 * 1. generate first platform in bottom right
 * 2. look from the "current" structure to what positions are viable
 * 3. generate two structures in a random position chosen from that pool in such a way that a structure goes "up" and another "to the right"
 * Viable positions are positions higher than the difficulty minimum, but also included in the player range
 * @param i 
 * @param j 
 */
// void World::fillRoom(byte i, byte j) {
//     int i_dist = 8, j_dist = 8; // how far we should generate
//     if(j < World::numCols / 8 - 1 && !this->roomsState[Pos(i, j + 1)].check()) {
//         j_dist += 8;
//     }

//     for(byte y = i + i_dist - 1; y <= i; --y) {
//         if(y == World::numRows - 1) {
//             // this case is different because we don't have to look downwards to get an idea of what we must generate
//             for(byte x = j; x < j + j_dist; ++j) {
//                 if(x == 0) {
//                     // leftmost, downward point should always be a platform
//                     this->generateLine(y, j, j + j_dist, x);
//                 }
//                 else {
//                     if(!this->tooClose(y, x)) {
                        
//                     }
//                 }
//             }
//         }
//     }
// }

void World::drawOnMatrix() {
    int row = 0;
    this->worldMap[Pos(this->player->getY(), this->player->getX())] = 1;

    for(int i = 8; i < 16; ++i) {
        int _col = 0;
        for(int j = 8; j < 16; ++j) {
            int actual_row = 7 - _col;
            int col = row; 
            this->lc->setLed(0, col, actual_row, this->worldMap[Pos(i, j)].check());
            _col++;
        }
        row += 1;
    }
}