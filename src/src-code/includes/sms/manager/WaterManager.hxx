#ifndef WATERMANAGER_H
#define WATERMANAGER_H

#include "types.h"
#include "sms/JUT.hxx"

class TWaterManager
{

public:
    u32 _00[0xC / 4];    //0x0000
    u8 mSprayQuality;    //0x000C
    u8 _01;              //0x000D
    u16 _02;             //0x000E
    s16 mMaxEmit;        //0x0010
    s16 mEmitCount;      //0x0012
    u32 _03[0x5D48 / 4]; //0x0014
    u16 _04;             //0x5D5C
    u8 _05;              //0x5D5E
    s8 mWaterCardType;   //0x5D5F

    struct
    {
        u8 _00 : 6;
        bool mMaskObjects : 1;
        bool mShowShadow : 1;
        u8 _01 : 8;
    } LightType; //0x5D60

    u16 _06;                 //0x5D62
    u32 _07[0xA8 / 4];       //0x5D64
    f32 mSize;             //0x5E0C
    f32 _08[0x30 / 4];     //0x5E10
    f32 mSphereStep;       //0x5E40
    u8 mLayerCount;          //0x5E44
    u8 mDarkLevel;           //0x5E45
    u8 _09;                  //0x5E46
    JUtility::TColor mColor; //0x5E47
};

#endif