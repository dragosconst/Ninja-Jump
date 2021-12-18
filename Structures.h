#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_PAG_W 8
#define MIN_PAG_W 4

#define MAX_MOV_W 4
#define MOV_RANGE 8
#define MOV_LEFT -1
#define MOV_RIGHT 1

#include "Arduino.h"
#include "World.h"

enum StructureTypes {PagodaStruct, MovingPlatformStruct};
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

struct StructNode {
    BaseStructure* structure;
    StructNode* next;

    StructNode(BaseStructure* structure, StructNode* node) : structure(structure), next(node) {}
};

#endif