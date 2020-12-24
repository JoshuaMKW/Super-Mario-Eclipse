#ifndef POLLUTIONLAYER_H
#define POLLUTIONLAYER_H

#include "types.h"
#include "sms/pollution/PollutionPos.hxx"

#include "sms/j3d/Joint.hxx"

class TPollutionLayer : public TJointModel
{

public:
    u16 mPollutionEffect; //0x0030
};

#endif