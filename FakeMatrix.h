#ifndef FAKEMATRIX_H
#define FAKEMATRIX_H

#include "Arduino.h"

struct Pos {
  int i, j; 
  Pos(int i, int j) { this->i = i; this->j = j;} 
};


/**
 * @brief 
 * Wrapper class for matrix bits.
 * 
 */
class FakeBit {
private:
  byte* addr;
  byte bitPos; // which bit it occupies in the original byte
public:
  FakeBit(byte* add, byte bitPos);
  ~FakeBit() {}

  byte check() const { return (*this->addr) & (1 << this->bitPos);}
  FakeBit& operator=(const byte other); // assign 1 or 0 at the position pointed by bitPos
  FakeBit& operator=(const FakeBit& other);
};

/**
 * @brief 
 * Class for encoding a 0/1 matrix using bits.
 * For example, a 16x16 matrix would be encoded using 16x2 bytes. To save memory, we will store it as a continous list of bytes. To simulate
 * different lines, I will iterate by (columns in original matrix) / 8 in the list.
 */
class FakeMatrix {
private:
    int numRows, numCols;
    byte* matrix;
public:

    // numCols is supposed to be the number of byte columns - i.e. the "logical" matrix has 8 * numCols columns
    FakeMatrix() {}
    FakeMatrix(int numRows, int numCols);
    FakeMatrix(const FakeMatrix& other);
    ~FakeMatrix();

    FakeBit operator[](const Pos& pos);
};

#endif