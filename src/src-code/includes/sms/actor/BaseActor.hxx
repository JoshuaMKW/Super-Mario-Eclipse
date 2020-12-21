#ifndef BASEACTOR_H
#define BASEACTOR_H

#include "types.h"
#include "sms/JDrama.hxx"
#include "sms/actor/SpineBase.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/actor/MActor.hxx"

class THitActor : public JDrama::TActor
{

public:
    THitActor **mCollidingObjs; //0x0044
    s16 mNumObjs;               //0x0048
    s16 mMaxObjs;               //0x004A
    u32 mObjectID;              //0x004C
    f32 mAttackRadius;          //0x0050
    f32 mAttackHeight;          //0x0054
    f32 mReceiveRadius;         //0x0058
    f32 mReceiveHeight;         //0x005C
    f32 _01;                    //0x0060
    u32 mObjectType;            //0x0064
};

class TTakeActor : public THitActor
{

public:
    TTakeActor *mHolder;     //0x0068
    TTakeActor *mHeldObject; //0x006C
};

class TLiveActor : public TTakeActor
{

public:
    u32 _00;                                   //0x0070
    MActor *mActorData;                        //0x0074
    u32 _01[0x14 / 4];                         //0x0078
    TSpineBase<TLiveActor> *mSpineBase;        //0x008C
    u32 _02;                                   //0x0090
    JGeometry::TVec3<f32> mPositionalVelocity; //0x0094
    JGeometry::TVec3<f32> mRotationalVelocity; //0x00A0
    JGeometry::TVec3<f32> mSpeed;              //0x00AC
    f32 mShadowRadius;                         //0x00B8
    f32 mMaxSpeed;                             //0x00BC
    f32 _03;                                   //0x00C0
    TBGCheckData *mFloorBelow;                 //0x00C4
    u32 _04;                                   //0x00C8
    f32 mGravity;                              //0x00CC 
    u32 _05[0x20 / 4];                         //0x00D0
};

#endif