#include "Arduino.h"
#include "World.h"

const byte World::numRows = 24;
const byte World::numCols = 24;

World::World() {
}

World::World(LedControl* lc, Player* player, byte difficulty) : lc(lc), player(player), difficulty(difficulty),
difficultyStepY(Player::getYRange() / 3), difficultyStepX(Player::getXRange() / 3) {
    this->worldMap = FakeMatrix(World::numRows, World::numCols / 8);
    this->first = nullptr;
    this->generateFromLast(true);
    this->generateFromLast();
    this->generateFromLast();
    // Serial.println(this->difficultyStepX);
    // Serial.println(this->difficultyStepY);
    // Serial.println(this->difficulty);
    // for(int i = 0; i < World::numRows; ++i) {
    //     for(int j = 0; j < World::numCols; ++j) {
    //         Serial.print(this->worldMap[Pos(i, j)].check() != 0);
    //         Serial.print(" ");
    //     }
    //     Serial.println();
    // }
    //     Serial.print(this->last.x); Serial.print(" "); Serial.print(this->last.y); Serial.println();
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
    this->first = other.first;
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
    this->first = other.first;
}

void World::scrollUp() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform(false);
    for(int i = World::numRows - 2; i >= 0; --i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i + 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    if(this->worldMap[this->player->getPos()].check()) {
        this->player->setPassedPlatform(true);
    }
    this->last.y += 1;
    this->secondLast.y += 1;
    this->player->setLastPos(Pos(this->player->getLastPos().i + 1, this->player->getLastPos().j));
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(0, j), B00000000);
    }
    this->moveAllStructsBy(1, 0);
    this->redrawStructs();
    this->checkCamera();
}

void World::scrollDown() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform(false);
    this->last.y -= 1;
    this->secondLast.y -= 1;
    this->player->setLastPos(Pos(this->player->getLastPos().i - 1, this->player->getLastPos().j));
    for(int i = 1; i < World:: numRows; ++i) {
        for(int j = 0; j < World::numCols; j += 8) {
            this->worldMap.setByte(Pos(i - 1, j), this->worldMap.getByte(Pos(i, j)));
        }
    }
    for(int j = 0; j < World::numCols; j += 8) {
        this->worldMap.setByte(Pos(World::numRows - 1, j), B00000000);
    }
    this->moveAllStructsBy(-1, 0);
    this->checkCamera();
}

void World::scrollRight() { 
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform(false);
    this->last.x -= 1;
    this->secondLast.x -= 1;
    this->player->setLastPos(Pos(this->player->getLastPos().i, this->player->getLastPos().j - 1));
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = 1; j < World::numCols; ++j) {
            this->worldMap[Pos(i, j - 1)] = this->worldMap[Pos(i, j)];
        }
    }    
    if(this->worldMap[this->player->getPos()].check()) {
        this->player->setPassedPlatform(true);
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, World::numCols - 1)] = 0;
    }
    this->moveAllStructsBy(0, -1);
    this->redrawStructs();
    this->checkCamera();
}

void World::scrollLeft() {
    if(!this->player->jumpedThroughPlatform())
        this->worldMap[this->player->getPos()] = 0;
    else
        this->player->setPassedPlatform(false);
    this->last.x += 1;
    this->secondLast.x += 1;
    this->player->setLastPos(Pos(this->player->getLastPos().i, this->player->getLastPos().j + 1));
    for(int i = 0; i < World::numRows; ++i) {
        for(int j = World::numCols - 2; j >= 0; --j) {
            this->worldMap[Pos(i, j + 1)] = this->worldMap[Pos(i, j)];
        }
    }
    if(this->worldMap[this->player->getPos()].check()) {
        this->player->setPassedPlatform(true);
    }
    for(int i = 0; i < World::numRows; ++i) {
        this->worldMap[Pos(i, 0)] = 0;
    }
    this->moveAllStructsBy(0, 1);
    this->redrawStructs();
    this->checkCamera();
}

bool World::intersect(BoundingBox s1, BoundingBox s2) {
    bool xTrue = false, yTrue = false;
    if(s1.x < s2.x && s1.x + s1.width >= s2.x) {
        xTrue = true;
    }
    if(s1.x >= s2.x && s2.x + s2.width >= s1.x) {
        xTrue = true;
    }
    if(s1.y < s2.y && s1.y + s1.height >= s2.y) {
        yTrue = true;
    }
    if(s1.y >= s2.y && s2.y + s2.height >= s1.y) {
        yTrue = true;
    }
    return xTrue && yTrue;
}

// this function will redraw structs that ended up outside the map matrix
// this is needed because structs will get quite big and there is no way to prevent them from disappearing anymore
void World::redrawStructs() {
    StructNode* current = this->first;
    while(current != nullptr) {
        BoundingBox box = current->structure->getBoundingBox();
        if(box.x <= 0 || box.y <= 0) {
            current->structure->draw(this);
        }
        current = current->next;
    }
}

void World::moveAllStructsBy(int8_t yVal, int8_t xVal) {
    StructNode* current = this->first;
    while(current != nullptr) {
        current->structure->setPos(Pos(current->structure->getPos().i + yVal, current->structure->getPos().j + xVal));
        current = current->next;
    }
}

// check if the last structure is now in the panning camera, in which case generate a new last structure
void World::checkCamera() {
    // if(this->last.x >= 8 && this->last.x < 16 && this->last.y >= 12 && this->last.y < 20) {
    if(this->intersect(last, BoundingBox(8, 12, 7, 7))) {
        // Serial.println("huh");
        // Serial.print(this->last.x); Serial.print(" "); Serial.print(this->last.y); Serial.print(" "); Serial.print(this->last.height); Serial.print(" "); Serial.print(this->last.width); Serial.println();
        // Serial.print(this->secondLast.x); Serial.print(" "); Serial.print(this->secondLast.y); Serial.print(" "); Serial.print(this->secondLast.height); Serial.print(" "); Serial.print(this->secondLast.width); Serial.println();
        //     for(int i = 0; i < World::numRows; ++i) {
        // for(int j = 0; j < World::numCols; ++j) {
        //     Serial.print(this->worldMap[Pos(i, j)].check() != 0);
        //     Serial.print(" ");
        // }
        // Serial.println();
    // }
        this->generateFromLast();
    }
}

// recenter the matrix so that pos becomes (12, 12)
void World::recenter(Pos pos) {
    Serial.print(pos.i); Serial.print(" "); Serial.println(pos.j);
    while(pos.j < PLAYER_X) {
        this->scrollLeft();
        pos.j += 1;
    }
    while(pos.j > PLAYER_X) {
        this->scrollRight();
        pos.j -= 1;
    }
    while(pos.i < PLAYER_Y) {
        this->scrollUp();
        pos.i += 1;
    }
    while(pos.i > PLAYER_Y) {
        this->scrollDown();
        pos.i -= 1;
    }
    this->player->setLastPos(pos);
}

BoundingBox World::generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor) {
    int8_t where = random(jst, jend);
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        this->worldMap[Pos(i, j)] = 1;
    }
    return BoundingBox(min(where, anchor), i, max(where, anchor) - min(where, anchor), 0);
}

// pointy lines look something like:
//*___*
//_***_
BoundingBox World::generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor) {
    int8_t where = random(jst, jend); 
    for(int8_t j = min(where, anchor); j <= max(where, anchor); ++j) {
        if(j == min(where, anchor) || j == max(where, anchor)) {
            this->worldMap[Pos(i - 1, j)] = 1;
        }
        else {
            this->worldMap[Pos(i, j)] = 1;
        }
    }
    return BoundingBox(min(where, anchor), i - 1, max(where, anchor) - min(where, anchor), 1);
}

BoundingBox World::generatePagoda(int8_t i, int8_t j) {
    StructNode* last = this->first;
    while(last != nullptr && last->next != nullptr) {
        last = last->next;
    }

    Pagoda* pagoda = new Pagoda(i, j);
    pagoda->draw(this);
    StructNode* newNode = new StructNode(pagoda, nullptr);
    if(last == nullptr) {
        this->first = newNode;
    }
    else {
        last->next = newNode;
    }

    return pagoda->getBoundingBox();
}

BoundingBox World::generateMovingPlatform(int8_t i, int8_t j) {
    StructNode* last = this->first;
    while(last != nullptr && last->next != nullptr) {
        last = last->next;
    }

    MovingPlatform* movingPlatform = new MovingPlatform(i, j);
    movingPlatform->draw(this);
    StructNode* newNode = new StructNode(movingPlatform, nullptr);
    if(last == nullptr) {
        this->first = newNode;
    }
    else {
        last->next = newNode;
    }

    return movingPlatform->getBoundingBox();
}

BoundingBox World::generateDisappPlatform(int8_t i, int8_t j) {
    StructNode* last = this->first;
    while(last != nullptr && last->next != nullptr) {
        last = last->next;
    }

    DisappearingPlatform* disappearingPlatform = new DisappearingPlatform(i, j);
    disappearingPlatform->draw(this);
    StructNode* newNode = new StructNode(disappearingPlatform, nullptr);
    if(last == nullptr) {
        this->first = newNode;
    }
    else {
        last->next = newNode;
    }

    return disappearingPlatform->getBoundingBox();
}

BoundingBox World::generateCanon(int8_t i, int8_t j) {
    StructNode* last = this->first;
    while(last != nullptr && last->next != nullptr) {
        last = last->next;
    }

    Canon* canon = new Canon(i, j, random(1, 4));
    canon->draw(this);
    StructNode* newNode = new StructNode(canon, nullptr);
    if(last == nullptr) {
        this->first = newNode;
    }
    else {
        last->next = newNode;
    }

    return canon->getBoundingBox();
}

// generate a structure in given params
BoundingBox World::generateStructure(int8_t x_first, int8_t y_first, int8_t x_last, int8_t y_last, int8_t xMax, int8_t yMax) {
    byte x = random(x_first, x_last), y = random(y_first, y_last); // anchor point of structure
    // Serial.print("X_first is "); Serial.print(x_first); Serial.print(" x_last is "); Serial.print(x_last); Serial.print(" y_first is "); Serial.print(y_first); Serial.print(" y_last is "); Serial.println(y_last);
    // Serial.print("y is "); Serial.println(y);
    if(y == World::numRows - 1) {
        return this->generateLine(y, x_first, xMax, x);
    }
    else {
        byte dice = random(GEN_TYPES);
        if(dice == GEN_LINE) {
           return this->generateLine(y, x_first, xMax, x);
        }
        else if(dice == GEN_POINTY) {
            return this->generatePointyLine(y, x_first, xMax, x);
        }
        else if(dice == GEN_PAGODA) {
            return this->generatePagoda(y, x);
        }
        else if(dice == GEN_MOVING) {
            return this->generateMovingPlatform(y, x);
        }
        else if(dice == GEN_DISAPP) {
            // disappearing platforms have preset shapes that require a certain positioning, relative to the last generated structure
            return this->generateDisappPlatform(y_first, x_first + 4);
        }
        else if(dice == GEN_CANON) {
            return this->generateCanon(y, x);
        }
    }

}

// get range from highest point to the left
BoundingBox World::getBestRange(BoundingBox structure) {
    int8_t topy = 100, leftx = 100, height, width;
    for(int8_t y = structure.y; y <= structure.y + structure.height; ++y) {
        for(int8_t x = structure.x; x <= structure.x + structure.width; ++x) {
            if(this->worldMap[Pos(y, x)].check() || structure.fillOnes) {

                if(topy > max(y - Player::maxJump / Player::jumpInterval + 1, 0)) {
                    topy = max(y - Player::maxJump / Player::jumpInterval + 1, 0);
                    height = y - topy;
                    leftx = max(x - Player::maxJump / Player::moveIntervalInAir, 0);
                    width = min(2 * Player::maxJump / Player::moveIntervalInAir, World::numCols - 1 - leftx);
                }
            }
        }
    }
    return BoundingBox(leftx, topy, width, height);

}

BoundingBox World::getMinDiff(BoundingBox structure) {
    int8_t topy = 100, leftx = 100, height, width;
    for(int8_t y = structure.y; y <= structure.y + structure.height; ++y) {
        for(int8_t x = structure.x; x <= structure.x + structure.width; ++x) {
            if(this->worldMap[Pos(y, x)].check() || structure.fillOnes) {
                if(topy > max(y - this->difficultyStepY * this->difficulty, 0)) {
                    topy = max(y - this->difficultyStepY * this->difficulty, 0);
                    height = y - topy;
                    leftx = max(x -this->difficultyStepX * this->difficulty, 0);
                    width = min(2 * this->difficultyStepX * this->difficulty, World::numCols - 1 - leftx);
                }                
            }
        }
    }    
    return BoundingBox(leftx, topy, width, height);
}

// get total jump range from a point
BoundingBox World::getTotalRange(int8_t y, int8_t x) {
    int8_t left = max((x - Player::maxJump / Player::moveIntervalInAir), 0);
    int8_t right = min(x + Player::maxJump / Player::moveIntervalInAir, World::numCols - 1);
    int8_t up = max(y - Player::maxJump / Player::jumpInterval, 0);
    return BoundingBox(left, up, right - left, y - up);
}

// get area which is reachable from last generated structure, but not from second last structure
// it's going to be a rought estimate, since we need a rectangle
BoundingBox World::withoutIntersection(BoundingBox s1, BoundingBox s2) {
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
        return BoundingBox(x, y, w, wh);
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

        return BoundingBox(x - w, y, w, wh);
    }
}

void World::generateFromLast(bool first = false) {
    if(first) {
        this->secondLast = BoundingBox(-100, -100, -100, -100);
        this->last = this->generateLine(PLAYER_Y + 1, 8, 16, PLAYER_X);
        this->player->setPosition(Pos(PLAYER_Y, PLAYER_X));
    }
    else {
        if(this->secondLast.x == -100) {
            // no second last yet, can generate anywhere within range for the first structure
            this->secondLast = this->last;
            // Serial.print("last is: x = "); Serial.print(last.x); Serial.print(" y = "); Serial.print(last.y); Serial.print(" height = "); Serial.print(last.height); Serial.print(" width = "); Serial.println(last.width);
            BoundingBox range = this->getBestRange(this->last);
            range = this->withoutIntersection(range, this->getMinDiff(this->last));
            // Serial.print("range is: x = "); Serial.print(range.x); Serial.print(" y = "); Serial.print(range.y); Serial.print(" height = "); Serial.print(range.height); Serial.print(" width = "); Serial.println(range.width);
            this->last = this->generateStructure(range.x, range.y, min(range.x + range.width + 1, World::numCols - 1), 
            min(range.y + range.height + 1, World::numRows - 1), min(range.x + range.width + 1 + 8, World::numCols - 1),  min(range.y + range.height + 1, World::numRows - 1));
        }
        else {
            BoundingBox rangeLast = this->getBestRange(this->last);
            // Serial.print("range last is: x = "); Serial.print(rangeLast.x); Serial.print(" y = "); Serial.print(rangeLast.y); Serial.print(" height = "); Serial.print(rangeLast.height); Serial.print(" width = "); Serial.println(rangeLast.width);
            BoundingBox rangeSecondLast = this->getBestRange(this->secondLast);
            // Serial.print("range second last is: x = "); Serial.print(rangeSecondLast.x); Serial.print(" y = "); Serial.print(rangeSecondLast.y); Serial.print(" height = "); Serial.print(rangeSecondLast.height); Serial.print(" width = "); Serial.println(rangeSecondLast.width);
            BoundingBox range = this->withoutIntersection(rangeLast, rangeSecondLast);                        
            // Serial.print("range is: x = "); Serial.print(range.x); Serial.print(" y = "); Serial.print(range.y); Serial.print(" height = "); Serial.print(range.height); Serial.print(" width = "); Serial.println(range.width);
            range = this->withoutIntersection(range, this->getMinDiff(this->last));
            this->secondLast = this->last;
            this->last = this->generateStructure(range.x, range.y, min(range.x + range.width + 1, World::numCols - 1), 
            min(range.y + range.height + 1, World::numRows - 1), min(range.x + range.width + 1 + 8, World::numCols - 1), min(range.y + range.height + 1, World::numRows - 1));
        }
    }
}

void World::drawOnMatrix() {
    int row = 0;
    this->worldMap[Pos(this->player->getY(), this->player->getX())] = 1;

    for(int i = 12; i < 20; ++i) {
        int _col = 0;
        for(int j = 8; j < 16; ++j) {
            int actual_row = _col;
            int col = row; 
            this->lc->setLed(0, col, actual_row, this->worldMap[Pos(i, j)].check());
            _col++;
        }
        row += 1;
    }
}

// called on object destructor, free all structures
void World::freeAllStructures() {
    while(this->first) {
        StructNode* toFree = this->first;
        this->first = this->first->next;
        delete toFree->structure;
        delete toFree;
    }
}

// free the structures that were "passed", i.e. they are below the last line or column
void World::freeStructures() {
    StructNode* current = this->first;
    while(current != nullptr) {
        if(current == this->first) {
            BoundingBox coords = current->structure->getBoundingBox();
            if(coords.y >= World::numRows || coords.x >= World::numCols) {
                StructNode* toFree = current;
                this->first = this->first->next;
                current = current->next;
                delete toFree->structure;
                delete toFree;

                continue;
            }
        }
        if(current->next != nullptr) {
            BoundingBox coords = current->next->structure->getBoundingBox();
            if(coords.y >= World::numRows || coords.x >= World::numCols) {
                StructNode* toFree = current->next;
                current->next = current->next->next;
                delete toFree->structure;
                delete toFree;
            }
        }
        current = current->next;
    }
}

void World::activateStructures() {
    StructNode* current = this->first;
    while(current) {
        current->structure->activate(this->player);
        current = current->next;
    }
}