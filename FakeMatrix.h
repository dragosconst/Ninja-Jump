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
 * While having a pointer and a byte for a bit is a 16x8=128 increase in memory usage!, this class is intended to be used only
 * for program logic and not as a memory unit in the FakeMatrix class or elsewhere, therefore it's very unlikely for this approach
 * to cause any sort of problems. There are few instances in which you'd need more than one FakeBit variable in memory at a time.
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
 * This class has the nice advantage of being usable just like a regular matrix, with the slight modification that indexing is done using
 * the Pos class, instead of two indexes like with regular matrices. So a[i][j] would be a[Pos(i, j)], while slightly different, it's pretty
 * much the same structure. 
 * Assignment, on the other hand, works exaclty the same: a[i][j] = x would be a[Pos(i,j)] = x. It's also possible to chain assignments, like
 * a[Pos(i, j)] = b[Pos(i, j)] = x.
 * The only "big" difference is, since the element returned by the index operator is a FakeBit, to check whether it's 1 or 0, you have to use the
 * check() method. So this piece of code:
 * if(a[i][j])
 *  do stuff;
 * becomes:
 * if(a[Pos(i,j)].check())
 *  do stuff;
 * Again, slightly different, but the same structure.
 */
class FakeMatrix {
private:
    byte numRows, numCols;
    byte* matrix;
public:

    // numCols is supposed to be the number of byte columns - i.e. the "logical" matrix has 8 * numCols columns
    FakeMatrix() {}
    FakeMatrix(byte numRows, byte numCols);
    FakeMatrix(const FakeMatrix& other);
    ~FakeMatrix();

    void setByte(const Pos& pos, const byte& b);
    FakeBit operator[](const Pos& pos);
};

#endif