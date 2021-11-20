#pragma once

#include "types.h"

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/collision/MapCollisionData.hxx"

class TMapWarp {

public:
  u32 _0;
  u32 _4;
  u32 _8;
  f32 _C; // init'd to 3.0

  class TMapWarpInfo {};
};

class TMapXlu {

public:
  u32 _0;
  u32 _4;
};

class TMap : public JDrama::TViewObj {
public:
  TMap(const char *);
  virtual ~TMap();

  virtual void load(JSUMemoryInputStream &stream) override;
  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  void changeModel(s16 index) const;
  f32 checkGround(f32 x, f32 y, f32 z, TBGCheckData **out) const;
  f32 checkGround(const JGeometry::TVec3<f32> &pos, TBGCheckData **out) const;
  f32 checkGroundExactY(f32 x, f32 y, f32 z, TBGCheckData **out) const;
  f32 checkGroundIgnoreWaterSurface(f32 x, f32 y, f32 z,
                                    TBGCheckData **out) const;
  f32 checkGroundIgnoreWaterSurface(const JGeometry::TVec3<f32> &pos,
                                    TBGCheckData **out) const;
  f32 checkGroundIgnoreWaterThrough(f32 x, f32 y, f32 z,
                                    TBGCheckData **out) const;
  f32 checkRoof(f32 x, f32 y, f32 z, TBGCheckData **out) const;
  f32 checkRoof(const JGeometry::TVec3<f32> &pos, TBGCheckData **out) const;
  f32 checkRoofIgnoreWaterThrough(f32 x, f32 y, f32 z,
                                  TBGCheckData **out) const;
  void draw(u32, JDrama::TGraphics *) const;
  TBGCheckData *getIllegalCheckData();
  TBGCheckData *intersectLine(const JGeometry::TVec3<f32> &a,
                              const JGeometry::TVec3<f32> &b, bool,
                              JGeometry::TVec3<f32> *);
  bool isInArea(f32 x, f32 z) const;
  bool isTouchedOneWall(f32, f32, f32, f32) const;
  bool isTouchedOneWallAndMoveXZ(f32 *, f32, f32 *, f32) const;
  bool isTouchedWallsAndMoveXZ(f32 *, f32, f32 *, f32) const;
  void update();

  TMapCollisionData *mCollisionData; // 0x0010
  TMapWarp *mMapWarp;                // 0x0014
  TMapXlu *mMapXlu;                  // 0x0018
};