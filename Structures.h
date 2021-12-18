#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_PAG_W 8
#define MIN_PAG_W 4


#include "Arduino.h"
#include "World.h"

enum StructureTypes {PagodaStruct, MovingPlatform};
struct Structure;
class World;

// abstract class
class BaseStructure {
protected:
    int8_t ey, ex; // where it's supposed to reach, i.e. anchor point determined in the world class
    StructureTypes type;
public:
    BaseStructure(int8_t ey, int8_t ex, StructureTypes type);
    virtual ~BaseStructure() {}

    virtual void draw(World* world) = 0;
    virtual void doStuff() = 0;
    virtual Pos getPos() const = 0;
    virtual void setPos(Pos pos) = 0;
    virtual Structure getBoundingBox() = 0;
};

class Pagoda : public BaseStructure {
    int8_t top, left, width, height;
public:
    Pagoda(int8_t ey, int8_t ex);
    ~Pagoda() {}

    void draw(World* world);
    void doStuff() { }
    Pos getPos() const { return Pos(this->top, this->left);}
    void setPos(Pos pos) { this->top = pos.i; this->left = pos.j;}
    Structure getBoundingBox();
};

struct StructNode {
    BaseStructure* structure;
    StructNode* next;

    StructNode(BaseStructure* structure, StructNode* node) : structure(structure), next(node) {}
};

#endif