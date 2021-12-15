#ifndef WORLD_H
#define WORLD_H

#define GEN_LINE 1
#define GEN_POINTY 2
#define GEN_PAGODA 3
#define GEN_MOVING 4
#define GEN_DISAPP 5
#define GEN_CANON 6

#include "Arduino.h"
#include "Player.h"
#include <LedControl.h>
#include "FakeMatrix.h"

class Player;

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

    void generateLine(byte i, byte jst, byte jend, byte anchor);
    void generatePointyLine(byte i, byte jst, byte jend, byte anchor);
    void generateStructure(byte xFirst, byte yFirst, byte xLast, byte yLast, byte xMax, byte yMax);
    bool tooClose(byte i, byte j);
    Pos getBestRange(byte i, byte j);
    void fillRoom(byte i, byte j);
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