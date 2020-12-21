#ifndef NOZZLEBASE_H
#define NOZZLEBASE_H

#include "types.h"

class TNozzleBase
{

public:
    u32 _00[0x2C / 4];       //0x0000
    f32 mEmitSize;           //0x002C
    u32 _01[0x24 / 4];       //0x0030
    f32 mEmitRandom;         //0x0054
    u32 _02[0x10 / 4];       //0x0058
    f32 mDistance;           //0x0068
    u32 _03[0x38 / 4];       //0x006C
    f32 mMasterScale;        //0x00A4
    u32 _04[0x10 / 4];       //0x00A8
    f32 mDistortionStrength; //0x00B8
    u32 _05[0x10 / 4];       //0x00BC
    s32 mMaxWater;           //0x00CC
    u32 _06[0x38 / 4];       //0x00D0
    s16 mWaterStep;          //0x0108
    u32 _10[0x278 / 4];      //0x010C
};

#endif