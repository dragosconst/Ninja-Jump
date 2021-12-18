#include "Arduino.h"
#include "Structures.h"

BaseStructure::BaseStructure(int8_t ey, int8_t ex, StructureTypes type) : ex(ex), ey(ey), type(type) {}

Pagoda::Pagoda(int8_t ey, int8_t ex) : BaseStructure(ey, ex, PagodaStruct) {
    this->width = random(MIN_PAG_W, MAX_PAG_W);
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
    this->length = random(1, MAX_MOV_W);
    this->crx = this->ex - this->length + 1;
    this->moveDirection = MOV_LEFT;
}

void MovingPlatform::draw(World* world) {
    int8_t y = this->ey;
    for(int8_t x = this->ex - MOV_RANGE + 1; x <= this->ex; ++x) {
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