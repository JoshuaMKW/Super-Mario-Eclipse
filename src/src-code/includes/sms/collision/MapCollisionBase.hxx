#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"

#include "sms/actor/LiveActor.hxx"
#include "sms/collision/BGCheck.hxx"

class TMapCollisionBase
{
public:
    void init(const char *, u16, const TLiveActor *);
    void initAllCheckData(s16, const f32 *, u16, const TLiveActor *);
    void initCheckData(s16, u16, const TLiveActor *);
    bool isSetUp() const;
    void moveMtx(float (*) [4]);
    void moveSRT(const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &);
    void moveTrans(const JGeometry::TVec3<f32> &);
    void remove();
    void setAllActor(const TLiveActor *);
    void setAllBGType(u16);
    void setAllData(s16);
    void setCheckData(const f32 *, const s16 *, TBGCheckData *, int);
    void setMtx(float (*) [4]);
    void setUp();
    void setUpTrans(const JGeometry::TVec3<f32> &);
    void setVertexData(u32, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &);
    void update();
    void updateTrans(const JGeometry::TVec3<f32> &);

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