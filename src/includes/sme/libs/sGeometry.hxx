#pragma once

#include "MTX.h"
#include "sMath.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "types.h"

namespace SME::Class {

class TVectorTriangle {
public:
  JGeometry::TVec3<f32> a;
  JGeometry::TVec3<f32> b;
  JGeometry::TVec3<f32> c;

  TVectorTriangle(){};
  TVectorTriangle(const JGeometry::TVec3<f32> &a,
                  const JGeometry::TVec3<f32> &b,
                  const JGeometry::TVec3<f32> &c);

  JGeometry::TVec3<f32> center();
  JGeometry::TVec3<f32> normal(bool normalize = true);
  f32 yPosAtXZ(f32 x, f32 z);

  static f32 bearingAngleY(const JGeometry::TVec3<f32> &a,
                           const JGeometry::TVec3<f32> &b);
  static JGeometry::TVec3<f32> cross(const JGeometry::TVec3<f32> &a,
                                     const JGeometry::TVec3<f32> &b);
  static f32 dot(const JGeometry::TVec3<f32> &a,
                 const JGeometry::TVec3<f32> &b);
};

} // namespace SME::Class
