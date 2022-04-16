#pragma once

#include "LiveActor.hxx"

#include "types.h"

template <typename T>
class TSpineBase;
template <typename T>
class TNerveBase{
    public:
    TNerveBase<T>(){};
    TNerveBase<T>(TNerveBase<T>& t){};
    virtual ~TNerveBase();
    virtual bool execute(TSpineBase<T> *mSpineBase) const;
};

template <typename T>
class TSpineBase
{

public:
    T *mTarget;        //0x0000
    u32 _00;           //0x0004
    u32 mVTableIndex;  //0x0008
    TNerveBase<T> **_01;  //0x000C
    void *mNerveStack;
    void *mCurVTable;  //0x0014
    void *mVTableCopy; //0x0018
    void *mPrevVTable; //0x001C
    u32 mStateTimer;   //0x0020
    void *mVTable;     //0x0024
};