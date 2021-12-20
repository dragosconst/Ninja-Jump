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
    this->state = false;
    this->playedSound = false;
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
        this->playedSound = false;
    }
    else if(millis() - this->lastSwitch >= DisappearingPlatform::switchInterval - DP_ALERT && !playedSound) {
        SoundsManager::playDisappearing();
        this->playedSound = true;
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

const int Canon::moveInterval = 50;
const int Canon::shootInterval = 300;

Canon::Canon(int8_t ey, int8_t ex, byte can_no) : BaseStructure(ey, ex, CanonStruct), can_no(can_no) {
    this->x[0] = Player::maxJump / Player::moveIntervalInAir;
    for(byte i = 1; i < MAX_SUBS + 1; ++i) {
        this->x[i]  = random(Player::maxJump / Player::moveIntervalInAir);
    }
    for(byte i = 0; i < MAX_SUBS; ++i) {
        this->b[i] = CAN_RANGE - 1; // initialize bullet positions to last collumn
    }
    int8_t can_no_cpy = can_no;
    // Serial.print("canon size is ");Serial.println(can_no);
     
    
    this->isShooting = false;
    this->lastMoved = millis();
    this->lastShot = millis();
}

int8_t Canon::translateBulletPos(int8_t x) {
    return ex - (CAN_RANGE - x - 1);
}

int8_t Canon::translatePlatPos(int8_t x) const {
    return ex - (CAN_PLAT_LEN - 1 + Player::maxJump / Player::moveIntervalInAir - x);
}

void Canon::draw(World* world) {
    int8_t top = this->ey - CAN_SUB_HEI * this->can_no;
    int8_t left = this->ex - CAN_PLAT_LEN + 1 - Player::maxJump / Player::moveIntervalInAir;
    int8_t row = -1; // -1 to skip the first row, which has only the final platform
    int8_t cr_x = can_no - 1; // we are drawing the structure top to bottom
    for(int8_t y = top; y < top + CAN_SUB_HEI * this->can_no + 1; ++y) {
        for(int8_t x = left; x <= this->ex; ++x) {
            if(y < 0 || x < 0 || y >= World::numRows || x >= World::numCols) {
                continue;
            }
            if(y == top) {
                // draw the last platform at the top
                int8_t cr_pos = this->translatePlatPos(this->x[MAX_SUBS]);
                if(x >= cr_pos && x < cr_pos + CAN_PLAT_LEN) {
                    (*world->getMatrix())[Pos(y, x)] = 1;
                    continue;
                }
                (*world->getMatrix())[Pos(y, x)] = 0;
                continue;
            }
            else {
                if(row % 3 == 1) {
                    (*world->getMatrix())[Pos(y, x)] = 0;
                }
                else if(row % 3 == 2) {
                    // draw the corresponding platform
                    int8_t cr_pos = this->translatePlatPos(this->x[cr_x]);
                    if(x >= cr_pos && x < cr_pos + CAN_PLAT_LEN) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        continue;
                    }
                    (*world->getMatrix())[Pos(y, x)] = 0;
                }
                else if(row % 3 == 0) {
                    if(x == ex) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        continue;
                    }
                    int8_t bx = this->translateBulletPos(b[cr_x]);
                    if(x == bx) {
                        (*world->getMatrix())[Pos(y, x)] = 1;
                        continue;
                    }
                    (*world->getMatrix())[Pos(y, x)] = 0;
                }
                
            }
        }
        if(row >= 0 && row % 3 == 2)
            cr_x--;
        row++;
    }
}

void Canon::activate(Player* player) {
    // check if we hit the player
    if(this->isShooting) {
        // the first canon is the bottom one
        for(int8_t i = 0; i < MAX_SUBS && i < can_no; ++i) {
            int8_t xi = this->translateBulletPos(b[i]);
            int8_t yi = this->ey - CAN_SUB_HEI * this->can_no + 1 + i * 3;
            if(player->getPos().i == yi && player->getPos().j == xi) {
                player->die();
                return; // if the player dies, this memory will be freed and we might get undefined behaviour if we continue with the function
            }
        }
    }

    // do movement and reloading
    if(this->isShooting && millis() - this->lastMoved >= Canon::moveInterval) {
        if(this->b[0] == 0) {
            this->isShooting = false;
            for(int i = 0; i < MAX_SUBS; ++i) {
                this->b[i] = CAN_RANGE - 1;
            }
            this->lastShot = millis();
            this->draw(player->getWorld());
            return;
        }
        for(int i = 0; i < MAX_SUBS; ++i) {
            // update all canons positions, if we don't use 3 canons, we will do a few extra iterations, but honestly for at most 2 extra iterations no optimization is worth the effort
            this->b[i] -= 1;
        }
        this->lastMoved = millis();
        this->draw(player->getWorld());
    }
    else if(!this->isShooting && millis() - this->lastShot >= Canon::shootInterval) {
        this->isShooting = true;
        SoundsManager::playCanons();
        return;
    }
}

Pos Canon::getPos() const {
    return Pos(this->ey - CAN_SUB_HEI * this->can_no, this->translatePlatPos(this->x[MAX_SUBS]));
}

void Canon::setPos(Pos pos) {
    int8_t top = this->ey - CAN_SUB_HEI * this->can_no;
    int8_t x_pos = this->translatePlatPos(this->x[MAX_SUBS]);
    this->ey -= (top - pos.i);
    this->ex -= (x_pos - pos.j);
}

BoundingBox Canon::getBoundingBox() {
    int8_t top = this->ey - CAN_SUB_HEI * this->can_no;
    int8_t x_pos = this->translatePlatPos(this->x[MAX_SUBS]);
    return BoundingBox(x_pos, top, CAN_PLAT_LEN - 1, 0);
}