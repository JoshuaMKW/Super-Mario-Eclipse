#pragma once

#include "types.h"
#include "sms/actor/LiveActor.hxx"

class TLodAnmIndex
{
};

class TLodAnm
{
public:
    TLodAnm(TLiveActor *, const TLodAnmIndex *, int, f32);
    bool setBckAnm(int);
    bool setBtpAnm(int);

    TLiveActor *mLiveActor;
    TLodAnmIndex *mLodAnmIndex;
    int mIndex;
    f32 _0C;
    size_t mLodNum;
    s32 _14; //maybe type?
    s32 _18;
    s32 _1C;
};