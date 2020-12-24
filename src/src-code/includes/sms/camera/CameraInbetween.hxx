#ifndef CAMERAINBETWEEN_H
#define CAMERAINBETWEEN_H

#include "types.h"
#include "sms/JGeometry.hxx"

class TCameraInbetween
{

public:
    u32 _00[0x30 / 4];                   //0x0000
    JGeometry::TVec3<f32> mSubjectPos; //0x0030
};

#endif