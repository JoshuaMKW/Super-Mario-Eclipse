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
    // Flags are 0x4 = ignore water, 0x1 = ignore death planes
    f32 checkGround(f32 x, f32 y, f32 z, u8 ignoreFlags, const TBGCheckData **) const;
    f32 checkGroundList(f32 x, f32 y, f32 z, u8 ignoreFlags, const TBGCheckList *, const TBGCheckData **);
    f32 checkRoof(f32 x, f32 y, f32 z, u8, const TBGCheckData **) const;
    f32 checkRoofList(f32 x, f32 y, f32 z, u8, const TBGCheckList *, const TBGCheckData **);
    void checkWallList(const TBGCheckList *, const TBGWallCheckRecord *);
    void checkWalls(TBGWallCheckRecord *) const;
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
    size_t mFloorArraySize;           //0x001C
    u32 _20[0x8 / 4];              //0x0020
    TBGCheckData *mColTable;       //0x0028
    TBGCheckList **mConnectedTris; //0x002C ?
    TBGCheckListWarp *mColWarpTable; // 0x0030
    u32 _34;              //0x0034
    size_t mCheckDataLength;          //0x0038
    size_t mCheckDataLength2;                       //0x003C
    u16 _40;                       //0x0040
    u16 mEntries[256];             //0x0042
    u16 mEntryID;
    u32 _244;

    static TBGCheckData mIllegalCheckData;
};