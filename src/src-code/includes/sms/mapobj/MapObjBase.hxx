#ifndef MAPOBJBASE_H
#define MAPOBJBASE_H

#include "types.h"
#include "sms/actor/BaseActor.hxx"

class TMapObjBase : public TLiveActor
{

public:
    struct
    {
        u32 _00 : 7;
        bool mIsSunken : 1;
        u32 _01 : 1;
        bool mIsRide : 1;
        u32 _02 : 1;
        bool mIsCanTaken : 1;
        u32 _03 : 16;
        bool mIsShadowDead : 1;
        u32 _04 : 2;
        bool mIsObjDead : 1;
    } DrawInfo; //0x00F0

    u32 _00[0x8 / 4]; //0x00F4
    u16 mState;       //0x00FC
    u16 _01;          //0x00FE
    u32 _02;          //0x0100
    s32 mStateTimer;  //0x0104
    u32 mMapObjID;    //0x0134
};

#endif