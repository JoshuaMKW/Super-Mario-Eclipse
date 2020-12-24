#ifndef MAPCOLLISION_H
#define MAPCOLLISION_H

#include "types.h"
#include "MTX.h"
#include "sms/JGeometry.hxx"

#include "sms/actor/BaseActor.hxx"
#include "sms/collision/BGCheck.hxx"

class TMapCollisionBase
{
    u32 *vTable;
    u32 *_4;
    s32 _8;
    u32 mNumVerts;   // _10
    u32 mVertOffset; // _14
    u32 mGroupCount; // _18
    u32 *mGroups;    // _1C
    Mtx _20;
    JGeometry::TVec3<f32> _50;
    u16 _5C;
    u16 _5E; // padding
};

class TMapCollisionData
{

public:
    f32 _00;                     //0x0000
    f32 _01;                     //0x0004
    u32 _02[0x8 / 4];              //0x0008
    u32 mUnkSize;                  //0x0010
    u32 _03[0x8 / 4];              //0x0014
    u32 mFloorArraySize;           //0x001C
    u32 _04[0x8 / 4];              //0x0020
    TBGCheckData *mColTable;       //0x0028
    TBGCheckList **mConnectedTris; //0x002C ?
    u32 _05[0x8 / 4];              //0x0030
    u32 mCheckDataLength;          //0x0038
    u32 _06;                       //0x003C
    u16 _07;                       //0x0040
    u16 mEntries[256];             //0x0042
};

class TMapCollisionMove : TMapCollisionBase
{
};

class TMapCollisionStatic : public TMapCollisionBase
{
    TLiveActor* mActor; // _60
};

class TMapCollisionWarp : TMapCollisionBase
{
};

#endif