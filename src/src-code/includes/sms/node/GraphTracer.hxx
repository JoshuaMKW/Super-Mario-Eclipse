#ifndef GRAPHTRACER_H
#define GRAPHTRACER_H

#include "types.h"
#include "sms/node/GraphWeb.hxx"

class TGraphTracer
{

public:
    TGraphWeb *mGraph; //0x0000
    s32 mCurrentNode;  //0x0004
    s32 mPreviousNode; //0x0008
    f32 _00;
    f32 _01;
    f32 _02;
};

#endif