#pragma once

#include "types.h"
#include "sms/actor/LiveActor.hxx"

class TMapObjBase : public TLiveActor
{

public:
    u32 _00[0x8 / 4];  //0x00F4
    u16 mState;        //0x00FC
    u16 _01;           //0x00FE
    u32 _02;           //0x0100
    s32 mStateTimer;   //0x0104
    u32 _03[0x2C / 4]; //0x0108
    u32 mMapObjID;     //0x0134
};