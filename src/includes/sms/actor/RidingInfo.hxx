#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/HitActor.hxx"

struct TRidingInfo
{
    THitActor *mHitActor; // _0
    JGeometry::TVec3<f32> mActorRidingPos; // _4
};