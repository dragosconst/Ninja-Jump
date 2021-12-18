#ifndef WORLD_H
#define WORLD_H

#define GEN_LINE 0
#define GEN_POINTY 1
#define GEN_PAGODA 2
#define GEN_MOVING 3
#define GEN_DISAPP 4
#define GEN_CANON 5
#define PLAYER_X 12
#define PLAYER_Y 16

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>
#include "FakeMatrix.h"
#include "Structures.h"

class Player;
struct StructNode;

struct Structure {
    int8_t x, y, width, height;
    Structure() {}
    Structure(int8_t x, int8_t y, int8_t width, int8_t height) : x(x), y(y), width(width), height(height) {}
};

class World {
private:
    friend class Player;
    FakeMatrix worldMap;
    LedControl* lc;
    Player* player;
    Structure last, secondLast;
    byte difficulty;
    StructNode* first;

    byte difficultyStepY, difficultyStepX;

    Structure generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generatePagoda(int8_t i, int8_t j);
    Structure generateStructure(int8_t xFirst, int8_t yFirst, int8_t xLast, int8_t yLast, int8_t xMax, int8_t yMax);
    bool tooClose(int8_t i, int8_t j);
    Structure getBestRange(Structure structure);
    Structure getMinDiff(Structure structure);
    Structure getTotalRange(int8_t i, int8_t j);
    Structure withoutIntersection(Structure s1, Structure s2);
    bool intersect(Structure s1, Structure s2);
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
    World(const World& other);
    World& operator=(const World& other);
    ~World() {this->freeAllStructures();}

    void drawOnMatrix();
    void freeStructures();

    FakeMatrix* getMatrix() { return &this->worldMap;}
};

#endif