#ifndef SPINEENEMY_H
#define SPINEENEMY_H

#include "types.h"
#include "sms/actor/BaseActor.hxx"
#include "sms/node/GraphTracer.hxx"

class TSpineEnemy : public TLiveActor
{

public:
    u32 _00[0x34 / 4];          //0x00F0
    TGraphTracer *mGraphTracer; //0x0124
    u32 _01[0x28 / 4];          //0x0128
};

#endif