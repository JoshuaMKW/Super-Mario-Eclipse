#ifndef YOSHI_H
#define YOSHI_H

#include "types.h"
#include "sms/JDrama.hxx"
#include "sms/JGeometry.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/object/EggYoshi.hxx"

class TYoshi
{

public:
    enum TYPE
    {
        GREEN,
        ORANGE,
        PURPLE,
        PINK
    };

    enum STATE
    {
        EGG = 0,
        DROWNING = 3,
        DYING = 4,
        UNMOUNTED = 6,
        MOUNTED = 8
    };

    s8 mState;                          //0x0000
    u8 _00;                             //0x0001
    u16 mSubState;                      //0x0002 ??
    u32 _01;                            //0x0004
    s32 mMaxJuice;                      //0x0008
    s32 mCurJuice;                      //0x000C
    TMario *mMario;                     //0x0010
    u32 _02[0xC / 4];                   //0x0014
    JGeometry::TVec3<f32> mCoordinates; //0x0020
    u32 _03[0x8 / 4];                   //0x002C
    MActor *mAnimationTable;            //0x0034
    u32 _04[0x4C / 4];                  //0x0038
    f32 mRedComponent;                  //0x0084
    f32 mGreenComponent;                //0x0088
    f32 mBlueComponent;                 //0x008C
    u32 _05[0x28 / 4];                  //0x0090
    u8 mFlutterState;                   //0x00B8
    u8 _06;                             //0x00B9
    u16 mFlutterTimer;                  //0x00BA
    u16 mMaxFlutterTimer;               //0x00BC
    u16 _07;                            //0x00BE
    f32 mMaxVSpdStartFlutter;           //0x00C0
    f32 mFlutterAcceleration;           //0x00C4
    u32 _08[0x8 / 4];                   //0x00C8
    s8 mType;                           //0x00D0
    u8 _09;                             //0x00D1
    u16 _10;                            //0x00D2
    u32 _11[0x1C / 4];                  //0x00D4
    TEggYoshi *mEgg;                    //0x00F0

    bool isMounted();
    static bool isMounted(TMario *gpMario);
    bool isGreenYoshi();
    static bool isGreenYoshi(TMario *gpMario);
    bool isGreenYoshiMounted();
    static bool isGreenYoshiMounted(TMario *gpMario);
    static bool isGreenYoshiAscendingWater(TMario *gpMario);
};

#endif