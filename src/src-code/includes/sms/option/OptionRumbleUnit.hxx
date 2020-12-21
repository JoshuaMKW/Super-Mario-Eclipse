#ifndef TOPTIONRUMBLEUNIT_H
#define TOPTIONRUMBLEUNIT_H

#include "types.h"
#include "sms/J2D.hxx"

class TOptionRumbleUnit
{
public:
    J2DScreen *mScreen; // _0
    u32 *_4;            // TExPane*
    u8 _8;
    u8 _9;
    u8 _A; // padding?
    u8 _B; // ^^
    u32 _C;
    u32 *_10;   // TPatternAnmControl*
    u32 *_14;   // TPatternAnmControl*
    u32 *_18;   // some class
    u32 mState; // _1C
    u32 *_20;   // copy of _18
    f32 _24;

    /*
    f32 mAnimSpeed;      //0x0088
    f32 mAnimFrame;      //0x008C
    s32 mXCoordA;          //0x0090
    s32 mYCoordA;          //0x0094
    s32 mXCoordB;          //0x0098
    s32 mYCoordB;          //0x009C
    f32 mAnimSizeFactor; //0x00A0
    */
};

#endif