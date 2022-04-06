#pragma once

#include "JGeometry.hxx"
#include "MTX.h"
#include "sMath.hxx"
#include "types.h"

namespace SME::Class {

class TVectorTriangle {
public:
  TVec3f a;
  TVec3f b;
  TVec3f c;

  TVectorTriangle() = default;
  TVectorTriangle(const TVec3f &a,
                  const TVec3f &b,
                  const TVec3f &c);

  void center(TVec3f &out);
  void normal(bool normalize, TVec3f &out);
  f32 yPosAtXZ(f32 x, f32 z);

  static f32 bearingAngleY(const TVec3f &a,
                           const TVec3f &b);
};

} // namespace SME::Class
