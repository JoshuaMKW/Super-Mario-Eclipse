#pragma once

#include "types.h"
#include "sms/actor/SpineEnemy.hxx"

class TBaseNPC : public TSpineEnemy
{
public:
    u32 _00; //0x0150
    u32 _01; //0x0154
    THitActor *mPrevHolder; //0x0158
    u32 _02[0xD4 / 4]; //0x015C
};