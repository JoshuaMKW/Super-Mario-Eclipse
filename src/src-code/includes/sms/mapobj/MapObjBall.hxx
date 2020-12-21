#ifndef MAPOBJBALL_H
#define MAPOBJBALL_H

#include "types.h"
#include "sms/mapobj/MapObjGeneral.hxx"

class TMapObjBall : public TMapObjGeneral
{

public:
    u32 _00[0x40 / 4];           //0x0138
    f32 mKickedVSpeed;         //0x0178
    u32 _01[0x14 / 4];           //0x017C
    f32 mAdditionalHeldHeight; //0x0190
    u32 _02[2];                  //0x0194
};

#endif