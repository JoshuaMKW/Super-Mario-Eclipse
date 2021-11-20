#pragma once

#include "types.h"
#include "JSU/JSUMemoryStream.hxx"
#include "JGeometry.hxx"

#include "sms/actor/LiveActor.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/collision/MapCollisionBase.hxx"

class TMapCollisionData
{
public:
    TMapCollisionData();

    void addCheckDataToGrid(TBGCheckData *, int);
    void *allocCheckData(u32);
    void *allocCheckList(int, int);
    void checkGround(f32 x, f32 y, f32 z, u8, const TBGCheckData **) const;
    void checkGroundList(f32 x, f32 y, f32 z, u8, const TBGCheckList *, const TBGCheckData **);
    void checkRoof(f32 x, f32 y, f32 z, u8, const TBGCheckData **) const;
    void checkRoofList(f32 x, f32 y, f32 z, u8, const TBGCheckList *, const TBGCheckData **);
    void checkWallList(const TBGCheckList *, const void *); //TBGWallCheckRecord
    void checkWalls(void *) const; //TBGWallCheckRecord
    u16 getEntryID();
    bool getGridArea(const TBGCheckData *, int, int *, int *, int *, int *);
    void init(JSUMemoryInputStream &);
    void initMoveCollision();
    u32 intersectLine(const JGeometry::TVec3<f32> &, const JGeometry::TVec3<f32> &, bool, JGeometry::TVec3<f32> *) const;
    bool polygonIsInGrid(f32, f32, f32, f32, TBGCheckData *);
    void removeCheckListData(u16, s32);
    void updateCheckListNode(s32, s32, s32);

    f32 mAreaSizeX;                     //0x0000
    f32 mAreaSizeZ;                     //0x0004
    u32 _08[0x8 / 4];              //0x0008
    u32 mUnkSize;                  //0x0010
    u32 _14[0x8 / 4];              //0x0014
    u32 mFloorArraySize;           //0x001C
    u32 _20[0x8 / 4];              //0x0020
    TBGCheckData *mColTable;       //0x0028
    TBGCheckList **mConnectedTris; //0x002C ?
    u32 _30[0x8 / 4];              //0x0030
    u32 mCheckDataLength;          //0x0038
    u32 _3C;                       //0x003C
    u16 _40;                       //0x0040
    u16 mEntries[256];             //0x0042
    u16 mEntryID;
    u32 _244;

    static TBGCheckData mIllegalCheckData;
};