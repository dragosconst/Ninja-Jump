#include "Arduino.h"
#include "Structures.h"

BaseStructure::BaseStructure(int8_t ey, int8_t ex, StructureTypes type) : ex(ex), ey(ey), type(type) {}

Pagoda::Pagoda(int8_t ey, int8_t ex) : BaseStructure(ey, ex, PagodaStruct) {
    this->width = random(MIN_PAG_W, MAX_PAG_W + 1);
    if(this->width % 2) {
        this->width += 1;
    }
    this->height = this->width / 2;
    this->top = this->ey - this->height + 1;
    this->left = this->ex - this->width + 1;
}

BoundingBox Pagoda::getBoundingBox() {
    return BoundingBox(this->left, this->top, this->width - 1, this->height - 1);
}

void Pagoda::draw(World* world) {
    int8_t row = 0, col = 0;  
    // Serial.print("top = "); Serial.print(this->top); Serial.print(" left = "); Serial.print(this->left); Serial.print(" wdith = "); Serial.print(this->width); Serial.print(" height = "); Serial.println(this->height);
    for(int8_t y = this->top; y < this->top + this->height; ++y) {
        col = 0;
        for(int8_t x = this->left; x < this->left + this->width; ++x) {
            if(y < 0 || x < 0 || y >= World::numRows || x >= World::numCols) {
                col++;
                continue;
            }
            if(col == this->width / 2 - row || col == this->width / 2 - row - 1 || col == this->width / 2 + row || col == this->width / 2 + row - 1) {
                (*world->getMatrix())[Pos(y, x)] = 1;
            }
            col++;
        }
        Serial.println();
        row++;
    }
}

const int MovingPlatform::moveInterval = 450; // interval at which a platform moves

// ex + 2 because otherwise it's barely seeable on the screen
MovingPlatform::MovingPlatform(int8_t ey, int8_t ex) : BaseStructure(ey, ex + 2, MovingPlatformStruct), lastMoved(millis()) {
    this->length = random(1, MAX_MOV_W + 1);
    this->crx = random(this->ex - MOV_RANGE + 1, this->ex - this->length + 2);
    this->moveDirection = random(2);
    if(this->moveDirection == 0) {
        this->moveDirection = MOV_LEFT;
    }
    else {
        this->moveDirection = MOV_RIGHT;
    }
}

void MovingPlatform::draw(World* world) {
    int8_t y = this->ey;
    for(int8_t x = this->ex - MOV_RANGE + 1; x <= this->ex; ++x) {
        if(x < 0 || y < 0 || x >= World::numCols || y >= World::numRows) 
            continue;
        if(x >= this->crx && x < this->crx + this->length) {
            (*world->getMatrix())[Pos(y, x)] = 1;
        }
        else {
            (*world->getMatrix())[Pos(y, x)] = 0;
        }
    }
}

void MovingPlatform::activate(Player* player) {
    // first, change direction, if necessary
    if(this->moveDirection == MOV_LEFT && this->crx == this->ex - MOV_RANGE + 1) {
        this->moveDirection = MOV_RIGHT;
    }
    else if(this->moveDirection == MOV_RIGHT && this->crx == this->ex - this->length + 1) {
        this->moveDirection = MOV_LEFT;
    }

    if(millis() - this->lastMoved < MovingPlatform::moveInterval)
        return;
    this->lastMoved = millis();
    bool playerOnIt = false;
    if(!player->isJumping() && player->getPos().i == this->ey - 1 && player->getPos().j >= this->crx && player->getPos().j < this->crx + this->length) {
        playerOnIt = true;
    }
    if(this->moveDirection == MOV_LEFT) {
        if(playerOnIt) {
            player->move(-1, 0);
        }
        this->crx -= 1;
        this->draw(player->getWorld());
    }
    else {
        if(playerOnIt) {
            player->move(1, 0);
        }
        this->crx += 1;
        this->draw(player->getWorld());
    }
}

BoundingBox MovingPlatform::getBoundingBox() {
    return BoundingBox(this->ex - MOV_RANGE + 1, this->ey, MOV_RANGE - 1, 0, true);
}

const int DisappearingPlatform::switchInterval = 1500;

DisappearingPlatform::DisappearingPlatform(int8_t ey, int8_t ex) : BaseStructure(ey, ex, DisappearingStruct), lastSwitch(millis()) {
    this->dtype = random(DP_TYPES);
    if(this->dtype == DP_LINE) {
        this->top = ey;
        this->left = ex - LINE_LEN + 1;
    }
    else if(this->dtype == DP_STAIR) {
        this->top = ey - STAIR_HEI + 1;
        this->left = ex - STAIR_LEN + 1;
    }
    else if(this->dtype == DP_CLINE) {
        this->top = ey;
        this->left = ex - CLINE_LEN + 1;
    }
    else if(this->dtype == DP_CSTAIR) {
        this->top = ey - CSTAIR_HEI + 1;
        this->left = ex - CSTAIR_LEN + 1;
    }
    Serial.println("built object");
    Serial.print("top="); Serial.print(this->top); Serial.print(" left=");Serial.println(this->left);
    Serial.println(this->dtype);
    this->state = false;
}

void DisappearingPlatform::draw(World* world) {
    if(this->dtype == DP_LINE) {
        int8_t col = 0;
        for(int8_t x = this->left; x < this->left + LINE_LEN; ++x) {
            if(this->top < 0 || x < 0 || this->top >= World::numRows || x >= World::numCols) {
                col++;
                continue;
            }
            if(col % 4 == 0) {
                if(this->state) { // when state is true, draw odd segments
                    if((col / 4) % 2 == 0) {
                        (*world->getMatrix())[Pos(this->top, x)] = 1;
                        (*world->getMatrix())[Pos(this->top, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                else {
                    if((col / 4) % 2) {
                        (*world->getMatrix())[Pos(this->top, x)] = 1;
                        (*world->getMatrix())[Pos(this->top, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
            }
            (*world->getMatrix())[Pos(this->top, x)] = 0;
            ++col;
        }
    }
    else if(this->dtype == DP_STAIR) {
        int8_t row = 0, col = 0;
        for(int8_t y = this->top; y < this->top + STAIR_HEI; ++y) {
            col = 0;
            for(int8_t x = this->left; x < this->left + STAIR_LEN; ++x) {
                if(y < 0 || x < 0 || y >= World::numRows || x >= World::numCols) {
                    col++;
                    continue;
                }
                if(row % 4 == 0) {
                    if(this->state && col == 0) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                else if(row % 2 == 0) {
                    if(!this->state && col == 2) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                (*world->getMatrix())[Pos(y, x)] = 0;
                col++;
            }
            row++;
        }
    }
    else if(this->dtype == DP_CLINE) {
        int8_t row = 0, col = 0;
        for(int8_t y = this->top; y < this->top + CLINE_HEI; ++y) {
            col = 0;
            for(int8_t x = this->left; x < this->left + CLINE_LEN; ++x) { 
                if(y < 0 || x < 0 || y >= World::numRows || x >= World::numCols) {
                    col++;
                    continue;
                }
                if(this->state && row % 4 == 0) {
                    if(col == row ||
                    col == 8 - row) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                else if(!this->state && row % 2 == 0 && row % 4) {
                    if(col == row ||
                    col == 8 - row) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                (*world->getMatrix())[Pos(y, x)] = 0;
                col++;
            }
            row++;
        }
    }
    else if(this->dtype == DP_CSTAIR) {
        int8_t row = 0, col = 0;
        for(int8_t y = this->top; y < this->top + CSTAIR_HEI; ++y) {
            col = 0;
            for(int8_t x = this->left; x < this->left + CSTAIR_LEN; ++x) { 
                if(y < 0 || x < 0 || y >= World::numRows || x >= World::numCols) {
                    col++;
                    continue;
                }
                if(this->state && row % 4 == 0) {
                    if(col % 4 == 0 && col == abs(row - 4)) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue; 
                    }
                }
                else if(!this->state && row % 4 && row % 2 == 0) {
                    if(col % 2 == 0 && col % 4) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        (*world->getMatrix())[Pos(y, x + 1)] = 1;
                        ++x;
                        col += 2;
                        continue;
                    }
                }
                (*world->getMatrix())[Pos(y, x)] = 0;
                ++col;
            }
            row++;
        }
    }
}

void DisappearingPlatform::activate(Player* player) {
    if(millis() - this->lastSwitch >= DisappearingPlatform::switchInterval) {
        this->lastSwitch = millis();
        this->state = !this->state;
        this->draw(player->getWorld());
    }
}

void DisappearingPlatform::setPos(Pos pos) {
    this->top = pos.i;
    this->left = pos.j;
}

BoundingBox DisappearingPlatform::getBoundingBox() {
    if(this->dtype == DP_LINE) {
        return BoundingBox(this->left, this->top, 1, 0, true);
    }
    else if(this->dtype == DP_STAIR) {
        return BoundingBox(this->left, this->top, 1, 0, true);
    }
    else if(this->dtype == DP_CLINE) {
        return BoundingBox(this->left, this->top, 1, 0, true);
    }
    else if(this->dtype == DP_CSTAIR) {
        return BoundingBox(this->top, this->ex - 1, 1, 0, true);
    }
}