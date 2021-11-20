#pragma once

#include "RailNode.hxx"
#include "types.h"

class TGraphWeb {
public:
  u32 *mNodes;          // 0x0000
  TRailNode *mRailNode; // 0x0004
  s32 mNodeCount;       // 0x0008
  char *mRailName;      // 0x000C
  u32 _00[0x8 / 4];     // 0x0010

  TGraphWeb(TRailNode *, const char *, int);
  virtual ~TGraphWeb();

  void calcGraphDirection(int);
  void filterRailNode(u32, const TRailNode *, const TRailNode *,
                      TRailNode *) const;
  int findNearestNodeIndex(const JGeometry::TVec3<f32> &, u32) const;
  int findNearestVisibleIndex(const JGeometry::TVec3<f32> &, f32, f32, f32,
                              u32) const;
  int getAimToDirNextIndex(int, int, const JGeometry::TVec3<f32> &,
                           const JGeometry::TVec3<f32> &, u32) const;
  s16 getEscapeDirLimited(int, int, const JGeometry::TVec3<f32> &,
                          const JGeometry::TVec3<f32> &, f32, u32) const;
  int getEscapeFromMarioIndex(int, int, const JGeometry::TVec3<f32> &,
                              u32) const;
  Vec getNearestPosOnGraphLink(const JGeometry::TVec3<f32> &) const;
  int getNeighborNodeIndexByFlag(int, int, u32) const;
  s16 getRandomButDirLimited(int, int, const JGeometry::TVec3<f32> &,
                             const JGeometry::TVec3<f32> &, f32, u32) const;
  int getRandomNextIndex(int, int, u32) const;
  int getShortestNextIndex(int, int, u32) const;
  Vec indexToPoint(int) const;
  void initGoalIndex(const Vec &);
  bool isDummy() const;
  bool startIsEnd() const;
  void translateNodes(TRailNode *);
};