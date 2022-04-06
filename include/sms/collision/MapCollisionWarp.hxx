#pragma once

#include "types.h"
#include "JGeometry.hxx"

#include "sms/actor/LiveActor.hxx"
#include "sms/collision/MapCollisionBase.hxx"

class TMapCollisionWarp : TMapCollisionBase
{
public:
    TMapCollisionWarp();

    void init(const s8, u16, const TLiveActor *);
    void remove();
    void setUp();
    void setUpTrans(const TVec3f &);

    u16 _60;
    TLiveActor *mActor; //0x0064
};