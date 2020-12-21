#ifndef WATERGUN_H
#define WATERGUN_H

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/nozzle/NozzleBase.hxx"
#include "sms/nozzle/NozzleTrigger.hxx"

#include "sms/actor/Mario.hxx"

class TWaterGun
{

public:
    enum NOZZLETYPE
    {
        Spray,
        Rocket,
        Underwater,
        Yoshi,
        Hover,
        Turbo,
        Sniper
    };

    u32 _00[0x8 / 4];                      //0x0000
    TMario *mMario;                        //0x0008
    TNozzleBase mNozzleDeform;             //0x000C
    TNozzleTrigger mNozzleDeformBomb;      //0x0390
    TNozzleTrigger mNozzleRocket;          //0x0720
    TNozzleBase mNozzleUnderWater;         //0x0AB0
    TNozzleBase mNozzleYoshiDeform;        //0x0E34
    TNozzleTrigger mNozzleYoshiDeformBomb; //0x11B8
    TNozzleTrigger mNozzleHover;           //0x1548
    TNozzleTrigger mNozzleTurbo;           //0x18D8
    TNozzleBase *mNozzleList[6];           //0x1C68
    s32 mCurrentWater;                     //0x1C80
    u8 mCurrentNozzle;                     //0x1C84
    u8 mSecondNozzle;                      //0x1C85
    bool mIsEmitWater;                     //0x1C86
    u8 _13;                                //0x1C87
    u32 _14[0x8 / 4];                      //0x1C88
    JGeometry::TVec3<f32> mGeometry[10];   //0x1C90
    u32 _15;                               //0x1D08
    u32 *mEmitInfo;                        //0x1D0C
    char *mPrmPath;                        //0x1D14
    char *mRocketPath;                     //0x1D18
    u32 _16[0xA0 / 4];                     //0x1D1C
};

#endif