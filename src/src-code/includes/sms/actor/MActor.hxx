#pragma once

#include "types.h"

class MActorAnmData
{

public:
    enum LOOPTYPE
    {
        END,
        BEGIN,
        LOOP
    };

    u32 mAnimationID; //0x0000
    void *vTable;     //0x0004
    u8 mLoopType;     //0x0008
    u8 _00;           //0x0009
    u16 _01;          //0x000A
    u16 mNumFrames;   //0x000C
    u16 _02;          //0x000E
    f32 mFrameRate; //0x0010
    f32 mCurFrame;  //0x0014
};

class MActorAnmBck : public MActorAnmData
{
};

class MActor
{

public:
    u32 _00[0xC / 4];       //0x0000
    MActorAnmBck *mBckInfo; //0x000C
    u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32* _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u8 _38;
	u8 _39;
	u8 _3A; // padding?
	u8 _3B; // ^^
	u32 mLightID; // _3C
	u8 _40;
	u8 _41; // padding?
	u8 _42; // ^^
	u8 _43; // ^^
	u32 mLightType; // _44
};