#pragma once

#include "types.h"
#include "MTX.h"
#include "JGeometry.hxx"

#include "sms/actor/LiveActor.hxx"
#include "sms/collision/MapCollisionBase.hxx"

class TMapCollisionMove : TMapCollisionBase
{
public:
    TMapCollisionMove();

    void init(const s8, u16, const TLiveActor *);
    void init(u32, u16, s16, const TLiveActor *);
    void move();
    void moveMtx(float (*) [4]);
    void moveSRT(const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &);
    void moveTrans(const JGeometry::TVec3<f32> &);
    void setList();

    u32 _00;
    u32 _04;
    u32 _08;
    u32 _0C;
    u32 _10;
    u32 _14;
    u32 _18;
    u32 _1C;
    Mtx _20;
    f32 _50;
    f32 _54;
    f32 _58;
    u16 _5C;
};