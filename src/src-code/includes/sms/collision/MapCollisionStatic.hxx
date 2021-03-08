#pragma once

#include "types.h"

#include "sms/actor/LiveActor.hxx"
#include "sms/collision/MapCollisionBase.hxx"

class TMapCollisionStatic : public TMapCollisionBase
{
public:
    TMapCollisionStatic();

    void init(const s8, u16, const TLiveActor *);
    void remove();
    void setUp();

    TLiveActor *mActor; // _60
};