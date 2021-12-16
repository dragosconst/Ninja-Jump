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
    this->generateFrom(0, World::numRows - 1, 8, World::numRows - 9, -1);
    for(byte i = 15; i >= 9; --i) {
        for(byte j = 8; j < 16; ++j) {
            if(this->worldMap[Pos(i, j)].check()) {
                this->player->setPosition(Pos(i - 1, j));
            }
        }
    }
    Serial.println(this->difficultyStepX);
    Serial.println(this->difficultyStepY);
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
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
    return *this;
}
World::World(const World& other) {
    this->lc = other.lc;
    this->player = other.player;
    this->worldMap = other.worldMap;
    this->difficultyStepX = other.difficultyStepX;
    this->difficultyStepY = other.difficultyStepY;
    this->difficulty = other.difficulty;
}

void World::scrollUp() {
    for(int i = World::numRows - 2; i >= 0; --i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i + 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(0, j), B00000000);
    }
    this->emptyLinesUp += 1; 
}

void World::scrollDown() {
    for(int i = 1; i < World:: numRows; --i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i - 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(World::numRows - 1, j), B00000000);
    }
    this->emptyLinesDown += 1; 
}

void World::scrollLeft() { 
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = World::numCols - 2; j >= 0; --j) {
            this->worldMap[Pos(i, j + 1)] = this->worldMap[Pos(i, j)];
        }
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, 0)] = 0;
    }
    this->emptyColumnsLeft += 1;
}

void World::scrollRight() {
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 1; j < World::numCols; ++j) {
            this->worldMap[Pos(i, j - 1)] = this->worldMap[Pos(i, j)];
        }
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, World::numCols - 1)] = 0;
    }
    this->emptyColumnsRight += 1;
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
     Serial.println("----------------------------------------");
            for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    int8_t where = random(jst, jend - 1);
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        this->worldMap[Pos(i, j)] = 1;
    }
    Serial.println("line structure is between");
    Serial.print(min(where, anchor)); Serial.print(" "); Serial.print(max(where, anchor)); Serial.println();
    Serial.println("at line");
    Serial.println(i);
    delay(10);    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    return Structure(min(where, anchor), i, max(where, anchor) - min(where, anchor), 0);
}

// pointy lines look something like:
//*___*
//_***_
Structure World::generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor) {
     Serial.println("----------------------------------------");
            for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    int8_t where = random(jst, jend - 1); 
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        if(j == min(where, anchor) || j == max(where, anchor)) {
            this->worldMap[Pos(i - 1, j)] = 1;
        }
        else {
            this->worldMap[Pos(i, j)] = 1;
        }
    }
    Serial.println("pointy structure is between");
    Serial.print(min(where, anchor)); Serial.print(" "); Serial.print(max(where, anchor)); Serial.println();
    Serial.println("at line");
    Serial.println(i - 1);
    delay(10);
        for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    return Structure(min(where, anchor), i - 1, max(where, anchor) - min(where, anchor), 1);
}

// generate a structure in given params
Structure World::generateStructure(int8_t x_first, int8_t y_first, int8_t x_last, int8_t y_last, int8_t xMax, int8_t yMax) {
    byte x = random(x_first, x_last - 1), y = random(y_first, y_last - 1); // anchor point of structure
    if(y == 0) {
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
Pos World::getBestRange(byte i, byte j) {
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


// if num is a negative number, this will generate all possible positions
void World::generateFrom(int8_t x1, int8_t y1, int8_t x2, int8_t y2, int8_t num) {
    if(x1 < 0 || x1 >= World::numCols || x2 >=  World::numCols || y1 < 0 || y2 >= World::numCols || y2 < 0) {
            // not viable, have to check ONLY for this structure
    Serial.println("failed-------------------------------------------");
                for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println();
        Serial.println("failed");
    Serial.print(x1); Serial.print(" ");Serial.print(y1);Serial.print(" "); Serial.print(x2);Serial.print(" "); Serial.print(y2);Serial.print(" "); Serial.println();
        return;
    }
    if(!num) {
        Serial.println("huh?----------------");
        return;
    }
    if(this->tooClose(y2, x2)) {
        Serial.println("nope");
        Serial.print(x2); Serial.print(" "); Serial.print(y2); Serial.print(" "); Serial.println();
        return;
    }
    if(this->tooClose(y1, x1)) { // during other generating calls, the map was populated in such a way that the current position isn't valid anymore
        for(int y = y1; y >= y2; --y) {
            for(int x = x1; x <= x2; ++x) {
                if(!this->tooClose(y, x)) {
                    x1 = x;
                    y1 = y;
                    y = y2 - 1; // break upper loop too
                    break;
                }
            }
        }   
    }
            for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println("generating");
    Serial.print(x1); Serial.print(" ");Serial.print(y1);Serial.print(" "); Serial.print(x2);Serial.print(" "); Serial.print(y2);Serial.print(" "); Serial.println();
    // generate this structure
    Structure structure = this->generateStructure(x1, y1, x2, y2, min(x1 + 8, World::numCols - 1), max(y1 - 8, 0));
    delay(10);
    // Serial.println("------------------------------------------");
    // Serial.println("stuff");
    // for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    // generate next three structures
    // rightmost sturcture first
    Pos rightmost(-1, -1), upmost(100, 100);
    for(int8_t y = structure.y; y <= min(structure.y + structure.height, World::numRows - 1); ++y) {
        for(int8_t x = structure.x; x <= min(structure.x + structure.width, World::numCols - 1); ++x) {
            if(this->worldMap[Pos(y, x)].check()) {
                if(x > rightmost.j) {
                    rightmost.j = x;
                    rightmost.i = y;
                }
            }
        }
    }
    for(int8_t y = structure.y; y <= min(structure.y + structure.height, World::numRows - 1); ++y) {
        for(int8_t x = structure.x; x <= min(structure.x + structure.width, World::numCols - 1); ++x) {
            if(this->worldMap[Pos(y, x)].check()) {
                if(y < upmost.i) {
                    upmost.j = x;
                    upmost.i = y;
                }
            }
        }
    }    
    Serial.println("up--------------");
                for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println("generating");
    Serial.print(x1); Serial.print(" ");Serial.print(y1);Serial.print(" "); Serial.print(x2);Serial.print(" "); Serial.print(y2);Serial.print(" "); Serial.println();    
    Serial.println("incepem apel cu");
    Serial.print(upmost.j); Serial.print(" "); Serial.println(upmost.i - this->difficultyStepY * (this->difficulty + 1)); Serial.print(" ");
    Serial.print(min(upmost.j + Player::maxJump / Player::moveIntervalInAir, World::numCols - 1)); Serial.print(" ");
    Serial.print(max(upmost.i - Player::maxJump / Player::jumpInterval, 0)); Serial.println();
        Serial.println("up--------------");
                for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    this->generateFrom(upmost.j, upmost.i - this->difficultyStepY * (this->difficulty + 1) , min(upmost.j + Player::maxJump / Player::moveIntervalInAir, World::numCols - 1), max(upmost.i - Player::maxJump / Player::jumpInterval, 0), num - 1);

    
    Serial.println("right-------------------------------------------");                for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
                }
    Serial.println("going to the right");
    Serial.print(upmost.j); Serial.print(" "); Serial.println(upmost.i - this->difficultyStepY * (this->difficulty + 1)); Serial.println();
    this->generateFrom(rightmost.j + this->difficultyStepX * (this->difficulty + 1), rightmost.i, min(rightmost.j + Player::maxJump / Player::moveIntervalInAir, World::numCols - 1), max(rightmost.i - Player::maxJump / Player::jumpInterval, 0), num - 1);

    // for(int8_t y = structure.y; y <= min(structure.y + structure.height, World::numRows - 1); ++y) {
    //     for(int8_t x = structure.x; x <= min(structure.x + structure.width, World::numCols - 1); ++x) {
    //         if(this->worldMap[Pos(y, x)].check()) {
    //             if(x < leftmost.j) {
    //                 leftmost.j = x;
    //                 leftmost.i = y;
    //             }
    //         }
    //     }
    // }
    // this->generateFrom(leftmost.j - this->difficultyStepX * (this->difficulty + 1), leftmost.i, leftmost.j - Player::maxJump / Player::moveIntervalInAir, leftmost.i - Player::maxJump / Player::jumpInterval, num - 1);

    // upmost structure next
    // not viable, have to check ONLY for this structure
    Serial.println("copy-------------------------------------------");
                for(int i = 0; i < World::numRows; ++i) {
        for(int j = 0; j < World::numCols; ++j) {
            Serial.print(this->worldMap[Pos(i, j)].check() != 0);
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println();
    for(int8_t y = structure.y; y <= min(structure.y + structure.height, World::numRows - 1); ++y) {
        for(int8_t x = structure.x; x <= min(structure.x + structure.width, World::numCols - 1); ++x) {
            if(this->worldMap[Pos(y, x)].check()) {
                if(y < upmost.i) {
                    upmost.j = x;
                    upmost.i = y;
                }
            }
        }
    }}

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