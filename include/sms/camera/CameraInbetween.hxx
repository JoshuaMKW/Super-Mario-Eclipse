#pragma once

#include "types.h"
#include "JGeometry.hxx"

class TCameraInbetween
{

public:
    u32 _00[0x30 / 4];                 //0x0000
    TVec3f mSubjectPos; //0x0030
};