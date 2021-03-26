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
  TVectorTriangle(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b,
                  const JGeometry::TVec3<f32> &c);

  JGeometry::TVec3<f32> center();
  JGeometry::TVec3<f32> normal(bool normalize = true);
  f32 yPosAtXZ(f32 x, f32 z);

  static f32 bearingAngleY(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b);
  static JGeometry::TVec3<f32> cross(const JGeometry::TVec3<f32> &a,
                                     const JGeometry::TVec3<f32> &b);
  static f32 dot(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b);

  template <typename T> static T magnitude(const JGeometry::TVec3<T> &) {
    return (T)sqrtf((f32)(this->x * this->x) + (f32)(this->y * this->y) +
                    (f32)(this->z * this->z));
  }

  template <typename T> static T resultant(const JGeometry::TVec3<T> &) {
    return (T)(fabsf((f32)this->x) + fabsf((f32)this->y) + fabsf((f32)this->z));
  }

  template <typename T> static T getNormalAngle(const JGeometry::TVec3<T> &) {
    return (T)((180.0f / M_PI) * atan2f(this->x, this->z));
  }

  template <typename T> static JGeometry::TVec3<T> unitVector(const JGeometry::TVec3<T> &) {
    T len = this->resultant();
    TVec3<T> unit(this->x / len, this->y / len, this->z / len);
    return unit;
  }

  template <typename T> static JGeometry::TVec3<T> normalized(const JGeometry::TVec3<T> &) {
    T mag = this->magnitude();
    TVec3<T> unit(this->x / mag, this->y / mag, this->z / mag);
    return unit;
  }
};

} // namespace SME::Class
