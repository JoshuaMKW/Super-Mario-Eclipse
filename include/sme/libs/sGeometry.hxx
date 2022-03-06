#pragma once

#include "JGeometry.hxx"
#include "MTX.h"
#include "sMath.hxx"
#include "types.h"

namespace SME::Class {

class TVectorTriangle {
public:
  JGeometry::TVec3<f32> a;
  JGeometry::TVec3<f32> b;
  JGeometry::TVec3<f32> c;

  TVectorTriangle() = default;
  TVectorTriangle(const JGeometry::TVec3<f32> &a,
                  const JGeometry::TVec3<f32> &b,
                  const JGeometry::TVec3<f32> &c);

  void center(JGeometry::TVec3<f32> &out);
  void normal(bool normalize, JGeometry::TVec3<f32> &out);
  f32 yPosAtXZ(f32 x, f32 z);

  static f32 bearingAngleY(const JGeometry::TVec3<f32> &a,
                           const JGeometry::TVec3<f32> &b);
};

} // namespace SME::Class
