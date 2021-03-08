#pragma once

#include "types.h"
#include "sms/JDrama.hxx"

#include "sms/collision/BGCheck.hxx"
#include "sms/collision/MapCollisionData.hxx"

class TMapWarp
{

public:
    u32 _0;
    u32 _4;
    u32 _8;
    f32 _C; // init'd to 3.0

    class TMapWarpInfo
    {
    };
};

class TMapXlu
{

public:
    u32 _0;
    u32 _4;
};

class TMap : public JDrama::TViewObj
{

public:
    TMapCollisionData *mCollisionData; //0x0010
    TMapWarp *mMapWarp;                //0x0014
    TMapXlu *mMapXlu;                  //0x0018
};