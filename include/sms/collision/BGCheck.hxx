#pragma once

#include "JGeometry.hxx"
#include "types.h"

class TLiveActor;

class TBGCheckData {
public:
  TBGCheckData();

  f32 getActiveJumpPower() const;
  JGeometry::TVec3<f32> *getNormal() const;
  s32 getPlaneType();
  bool isIllegalData() const;
  bool isMarioThrough() const;
  bool isWaterSlip() const;
  bool isWaterSurface() const;
  void setVertex(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b,
                 const JGeometry::TVec3<f32> &c);

  u16 mCollisionType;             // 0x0000
  s16 mValue4;                    // 0x0002
  u16 mIllegalFlags;              // 0x0004 | 0x10 = illegal
  u8 mTerrainType;                // 0x0006 | Sound ID
  f32 mMinHeight;                 // 0x0008
  f32 mMaxHeight;                 // 0x000C
  JGeometry::TVec3<f32> mVertexA; // 0x0010
  JGeometry::TVec3<f32> mVertexB; // 0x001C
  JGeometry::TVec3<f32> mVertexC; // 0x0028
  JGeometry::TVec3<f32> mNormal;  // 0x0034
  f32 _40;
  TLiveActor *mOwner; // 0x0044
};

class TBGCheckList {
public:
  TBGCheckList();
  virtual void setPreNode(TBGCheckList *);

  TBGCheckList *mNextTriangle; // 0x0004
  TBGCheckData *mColTriangle;  // 0x0008
};

class TBGCheckListRoot {
public:
  TBGCheckListRoot();

  TBGCheckList mCheckLists[3]; // 0x0000
};

class TBGCheckListWarp : public TBGCheckList {
public:
  TBGCheckListWarp();
  virtual void setPreNode(TBGCheckList *);

  TBGCheckList *mPreNode; // _C
  u16 _10;
  u16 _12;
};