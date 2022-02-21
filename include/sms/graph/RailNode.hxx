#pragma once

#include "JGeometry.hxx"
#include "types.h"

class TRailNode {

public:
  JGeometry::TVec3<s16> mPosition; // 0x0000
  s16 mNeighborCount;              // 0x0006
  u32 mFlags;                      // 0x0008
  u32 _0C[2];                      // 0x000C
  s16 mNeighborIDs[8];             // 0x0014
  f32 mNeightborDistances[8];      // 0x0024
};