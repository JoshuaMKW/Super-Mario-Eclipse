#ifndef SPINEBASE_H
#define SPINEBASE_H

#include "types.h"

template <typename T>
class TSpineBase
{

public:
    T *mTarget;        //0x0000
    u32 _00;           //0x0004
    u32 mVTableIndex;  //0x0008
    u32 _01[0x8 / 4];  //0x000C
    void *mCurVTable;  //0x0014
    void *mVTableCopy; //0x0018
    void *mPrevVTable; //0x001C
    u32 mStateTimer;   //0x0020
    void *mVTable;     //0x0024
};

#endif