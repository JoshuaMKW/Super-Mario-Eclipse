#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"

class TRailNode
{

public:
    JGeometry::TVec3<s16> mPosition; //0x0000
    s16 mNeighborCount;              //0x0006
    u32 _00[0xC / 4];                //0x0008
    s16 mNeighborIDs[8];             //0x0014
    f32 mNeightborDistances[8];    //0x0024
};