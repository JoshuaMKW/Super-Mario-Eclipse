#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"

class TCameraInbetween
{

public:
    u32 _00[0x30 / 4];                 //0x0000
    JGeometry::TVec3<f32> mSubjectPos; //0x0030
};