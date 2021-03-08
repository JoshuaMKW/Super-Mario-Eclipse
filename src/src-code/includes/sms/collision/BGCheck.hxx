#pragma once

#include "types.h"
#include "sms/JGeometry.hxx"

class TLiveActor;

class TBGCheckData
{

public:
    u16 mCollisionType;             //0x0000
    s16 mValue4;                    //0x0002
    u16 _00;                        //0x0004
    u8 mTerrainType;                //0x0006
    u8 _01;                         //0x0007
    f32 mMinHeight;                 //0x0008
    f32 mMaxHeight;                 //0x000C
    JGeometry::TVec3<f32> mVertexA; //0x0010
    JGeometry::TVec3<f32> mVertexB; //0x001C
    JGeometry::TVec3<f32> mVertexC; //0x0028
    JGeometry::TVec3<f32> mNormal;  //0x0034
    TLiveActor *mOwner;             //0x0044
};

class TBGCheckList
{

public:
    u32 *mVTable;                //0x0000
    TBGCheckList *mNextTriangle; //0x0004
    TBGCheckData *mColTriangle;  //0x0008
};

class TBGCheckListRoot
{

public:
    TBGCheckList mCheckLists[3]; //0x0000
};

class TBGCheckListWarp : public TBGCheckList
{
    
public:
    TBGCheckList* mPreNode; // _C
    u16 _10;
    u16 _12;
};