#ifndef WORLD_H
#define WORLD_H

#define GEN_TYPES 6
#define GEN_LINE 0
#define GEN_POINTY 1
#define GEN_PAGODA 2
#define GEN_MOVING 3
#define GEN_DISAPP 4
#define GEN_CANON 5
#define PLAYER_X 12
#define PLAYER_Y 25
#define PAGODAS 200
#define MOVING 500
#define DISAPPEARING 800
#define CANON 1200

#define CAMERA_X 8
#define CAMERA_Y 21
#define CAMERA_DIM 8

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>
#include "FakeMatrix.h"
#include "Structures.h"

class Player;
struct StructNode;

struct BoundingBox {
    int8_t x, y, width, height;
    bool fillOnes; // pretend that the entire structure box contains ones
    BoundingBox() {}
    BoundingBox(int8_t x, int8_t y, int8_t width, int8_t height, bool fillOnes = false) : x(x), y(y), width(width), height(height), fillOnes(fillOnes) {}
};

class World {
private:
    friend class Player;
    friend class BaseStructure;
    FakeMatrix worldMap;
    LedControl* lc;
    Player* player;
    BoundingBox last, secondLast;
    byte difficulty;
    StructNode* first;
    bool justDisappeared;

    byte difficultyStepY, difficultyStepX;

    BoundingBox generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    BoundingBox generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    BoundingBox generatePagoda(int8_t i, int8_t j);
    BoundingBox generateMovingPlatform(int8_t i, int8_t j);
    BoundingBox generateDisappPlatform(int8_t i, int8_t j);
    BoundingBox generateCanon(int8_t i, int8_t j);
    BoundingBox generateStructure(int8_t xFirst, int8_t yFirst, int8_t xLast, int8_t yLast, int8_t xMax, int8_t yMax);
    BoundingBox getBestRange(BoundingBox structure);
    BoundingBox getMinDiff(BoundingBox structure);
    BoundingBox getTotalRange(int8_t i, int8_t j);
    BoundingBox withoutIntersection(BoundingBox s1, BoundingBox s2);
    bool intersect(BoundingBox s1, BoundingBox s2);
    void generateFromLast(bool first = false);
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void checkCamera();
    void recenter(Pos pos);
    void moveAllStructsBy(int8_t yVal, int8_t xVal);
    void redrawStructs();
    void freeAllStructures();
public:
    static const byte numRows, numCols;
    
    World();
    World(LedControl* lc, Player* player, byte difficulty);
    ~World() {this->freeAllStructures();}

    void drawOnMatrix();
    void activateStructures();
    void freeStructures();

    FakeMatrix* getMatrix() { return &this->worldMap;}
};

#endif