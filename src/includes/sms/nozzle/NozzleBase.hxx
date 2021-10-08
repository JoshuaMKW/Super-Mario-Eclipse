#pragma once

#include "types.h"

class TWaterGun;

class TNozzleBase
{

public:
    u32 _00[0x2C / 4];          //0x0000
    f32 mEmitPerFrame;          //0x002C
    u32 _30[0x24 / 4];          //0x0030
    f32 mEmitRandom;            //0x0054
    u32 _58[0x10 / 4];          //0x0058
    f32 mDistance;              //0x0068
    u32 _6C[0x38 / 4];          //0x006C
    f32 mMasterScale;           //0x00A4
    u32 _A8[0x10 / 4];          //0x00A8
    f32 mDistortionStrength;    //0x00B8
    u32 _BC[0x10 / 4];          //0x00BC
    s32 mMaxWater;              //0x00CC
    u32 _D0[0x10 / 4];          //0x00D0
    f32 mForwardSpeedFactor;    //0x00E0
    u32 _E4[0x24 / 4];
    s16 mWaterStep;             //0x0108
    u32 _10C[0x1DC / 4];        //0x010C
    s16 mMaxSprayQuarterFrames; //0x02E8
    u16 _2EA;
    u32 _2EC[0x7C / 4]; //0x02EC
    TWaterGun *mFludd;  //0x0368
    u32 _36C[0x18 / 4]; //0x036C
};