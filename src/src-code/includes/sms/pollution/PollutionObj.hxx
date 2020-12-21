#ifndef POLLUTIONOBJ_H
#define POLLUTIONOBJ_H

#include "types.h"
#include "sms/pollution/PollutionPos.hxx"

#include "sms/j3d/Joint.hxx"

class TPollutionObj : public TJointObj
{

public:
    u32 _00[0x10 / 4];       //0x0020
    u32 mCleanedDegree;      //0x0030
};

#endif