#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_PAG_W 8
#define MIN_PAG_W 4

#define MAX_MOV_W 6
#define MOV_RANGE 8
#define MOV_LEFT -1
#define MOV_RIGHT 1

#define DP_TYPES 4 // types of disappearing platforms
#define DP_LINE 0
#define DP_STAIR 1
#define DP_CLINE 2
#define DP_CSTAIR 3
#define DP_SIZE 2 // size of disappearing platform
#define LINE_LEN 14
#define LINE_HEI 1
#define STAIR_LEN 4
#define STAIR_HEI 7
#define CLINE_LEN 10
#define CLINE_HEI 5
#define CSTAIR_LEN 6
#define CSTAIR_HEI 9
#define DP_ALERT 400 // it emits a small alert, before disappearing

#define CAN_RANGE 8
#define MAX_CANONS 3
#define CAN_PLAT_LEN 2
#define CAN_SUB_HEI 3
#define MAX_SUBS 3

#include "Arduino.h"
#include "World.h"
#include "SoundsManager.h"

enum StructureTypes {PagodaStruct, MovingPlatformStruct, DisappearingStruct, CanonStruct};
struct BoundingBox;
class World;
class Player;

// abstract class
class BaseStructure {
protected:
    int8_t ey, ex; // where it's supposed to reach, i.e. anchor point determined in the world class
    StructureTypes type;
public:
    BaseStructure(int8_t ey, int8_t ex, StructureTypes type);
    virtual ~BaseStructure() {}

    virtual void draw(World* world) = 0;
    virtual void activate(Player* player) = 0;
    virtual Pos getPos() const = 0;
    virtual void setPos(Pos pos) = 0;
    virtual BoundingBox getBoundingBox() = 0;
};

class Pagoda : public BaseStructure {
    int8_t top, left, width, height;
public:
    Pagoda(int8_t ey, int8_t ex);
    ~Pagoda() {}

    void draw(World* world);
    void activate(Player* player) { }
    Pos getPos() const { return Pos(this->top, this->left);}
    void setPos(Pos pos) { this->top = pos.i; this->left = pos.j;}
    BoundingBox getBoundingBox();
};

class MovingPlatform : public BaseStructure {
private: 
    int8_t length, crx, moveDirection;
    long lastMoved;
public:
    static const int moveInterval;

    MovingPlatform(int8_t ey, int8_t ex);
    ~MovingPlatform() {}

    void draw(World* world);
    void activate(Player* player);

    Pos getPos() const { return Pos(this->ey, this->crx);}
    void setPos(Pos pos) { this->ey = pos.i; this->ex -= (this->crx - pos.j); this->crx = pos.j;}
    BoundingBox getBoundingBox();
};

/**
 * @brief 
 * Disappearing platforms will be spawned with different unique types, representing what pattern they will follow.
 */
class DisappearingPlatform : public BaseStructure { 
private:
    int8_t dtype, top, left, state;
    long lastSwitch;
    bool playedSound;
public:
    static const int switchInterval;

    DisappearingPlatform(int8_t ey, int8_t ex);
    ~DisappearingPlatform() {}

    void draw(World* world);
    void activate(Player* player);

    Pos getPos() const { return Pos(this->top, this->left);};
    void setPos(Pos pos);
    BoundingBox getBoundingBox();
};

/**
 * @brief 
 * Canons will be at most 3 layered substructures consisting of a platform of length 2, a canon and at the end another platform the player has
 * to jump to (or the final platform).
 */
class Canon : public BaseStructure {
private:
    byte can_no;
    int8_t x[MAX_SUBS + 1];
    int8_t b[MAX_SUBS]; // bullet positions on the map
    long lastMoved, lastShot;
    bool isShooting;

    int8_t translateBulletPos(int8_t x);
    int8_t translatePlatPos(int8_t x) const;
public:
    static const int moveInterval, shootInterval;

    Canon(int8_t ey, int8_t ex, byte can_no);
    ~Canon() {}

    void draw(World* world);
    void activate(Player* player);

    Pos getPos() const;
    void setPos(Pos pos);
    BoundingBox getBoundingBox();
};

struct StructNode {
    BaseStructure* structure;
    StructNode* next;

    StructNode(BaseStructure* structure, StructNode* node) : structure(structure), next(node) {}
};

#endif