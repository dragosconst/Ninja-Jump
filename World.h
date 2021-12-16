#ifndef WORLD_H
#define WORLD_H

#define GEN_LINE 0
#define GEN_POINTY 1
#define GEN_PAGODA 2
#define GEN_MOVING 3
#define GEN_DISAPP 4
#define GEN_CANON 5

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>
#include "FakeMatrix.h"

class Player;

struct Structure {
    byte x, y, width, height;
    Structure(byte x, byte y, byte width, byte height) : x(x), y(y), width(width), height(height) {}
};

class World {
private:
    friend class Player;
    FakeMatrix worldMap;
    FakeMatrix roomsState;
    LedControl* lc;
    Player* player;
    int emptyLinesUp, emptyLinesDown;
    int emptyColumnsLeft, emptyColumnsRight;
    byte difficulty;

    static const byte numRows, numCols;
    byte difficultyStepY, difficultyStepX;

    Structure generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generateStructure(int8_t xFirst, int8_t yFirst, int8_t xLast, int8_t yLast, int8_t xMax, int8_t yMax);
    bool tooClose(int8_t i, int8_t j);
    Pos getBestRange(byte i, byte j);
    void generateFrom(int8_t xFirst, int8_t yFirst, int8_t xLast, int8_t yLast, int8_t num);
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
public:
    World();
    World(LedControl* lc, Player* player, byte difficulty);
    World(const World& other);
    World& operator=(const World& other);
    ~World() {}

    void drawOnMatrix();
};

#endif