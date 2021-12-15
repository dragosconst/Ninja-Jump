#include "Arduino.h"
#include "FakeMatrix.h"

FakeBit::FakeBit(byte* addr,byte bitPos) : addr(addr), bitPos(bitPos) {}

FakeBit& FakeBit::operator=(const byte other) {
    if(other != 1 && other)
        return *this;
    if(other) {
        (*this->addr) |= (1 << this->bitPos);
    }
    else {
        if(this->check()){ // remove 1 from bit position
            (*this->addr) ^= (1 << this->bitPos);
        }
    }
    return *this;
}

FakeBit& FakeBit::operator=(const FakeBit& other) {
    if(other.check()) {
        (*this->addr) |= (1 << this->bitPos);
    }
    else {
        if(this->check()) {
            (*this->addr) ^= (1 << this->bitPos);
        }
    }
    return *this;
}

FakeMatrix::FakeMatrix(byte numRows, byte numCols) : numRows(numRows), numCols(numCols) {
    this->matrix = (byte*)malloc(numRows * numCols * sizeof(byte));
    this->matrix = {0}; // initialize with zeros
}

FakeMatrix::FakeMatrix(const FakeMatrix& other) {
    this->numRows = other.numRows;
    this->numCols = other.numCols;
    this->matrix = (byte*)malloc(this->numRows * this->numCols * sizeof(byte));
    for(int i = 0; i < this->numRows * this->numCols; ++i){
        this->matrix[i] = other.matrix[i];
    }
}

FakeMatrix& FakeMatrix::operator=(const FakeMatrix& other) {
    this->numRows = other.numRows;
    this->numCols = other.numCols;
    this->matrix = (byte*)malloc(this->numRows * this->numCols * sizeof(byte));
    for(int i = 0; i < this->numRows * this->numCols; ++i){
        this->matrix[i] = other.matrix[i];
    }
    return *this;
}

FakeMatrix::~FakeMatrix() {
    free(this->matrix);
}

void FakeMatrix::setByte(const Pos& pos, const byte& b) {
    int i = pos.i, j = pos.j;
    int index = i * this->numCols + j / 8;
    this->matrix[index] = b;
}

FakeBit FakeMatrix::operator[](const Pos& pos) {
    int i = pos.i, j = pos.j;
    int index = i * this->numCols + j / 8;
    // return this->matrix[index] & (1 << ( 7 - (j % 8)));
    return FakeBit(this->matrix + index, (7 - (j % 8)));
}