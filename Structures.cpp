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

Structure Pagoda::getBoundingBox() {
    return Structure(this->left, this->top, this->width - 1, this->height - 1);
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