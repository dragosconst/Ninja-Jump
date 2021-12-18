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

class Player;

struct Structure {
    int8_t x, y, width, height;
    Structure() {}
    Structure(int8_t x, int8_t y, int8_t width, int8_t height) : x(x), y(y), width(width), height(height) {}
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
    Structure last, secondLast;
    byte difficulty;

    static const byte numRows, numCols;
    byte difficultyStepY, difficultyStepX;

    Structure generateLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generatePointyLine(int8_t i, int8_t jst, int8_t jend, int8_t anchor);
    Structure generateStructure(int8_t xFirst, int8_t yFirst, int8_t xLast, int8_t yLast, int8_t xMax, int8_t yMax);
    bool tooClose(int8_t i, int8_t j);
    Structure getBestRange(byte i, byte j);
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
public:
    World();
    World(LedControl* lc, Player* player, byte difficulty);
    World(const World& other);
    World& operator=(const World& other);
    ~World() {}

    void drawOnMatrix();
};

#endif