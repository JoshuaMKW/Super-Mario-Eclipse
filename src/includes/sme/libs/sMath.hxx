#include "math.h"

#pragma once

#include "macros.h"
#include "sms/JSystem/JGeometry.hxx"
#include "types.h"

namespace SME::Util::Math {

constexpr f32 angleToRadians(f32 a) {
  return (static_cast<f32>(M_PI) / 180.0f) * a;
}
constexpr f64 angleToRadians(f64 a) { return (M_PI / 180.0) * a; }
constexpr f32 radiansToAngle(f32 r) {
  return (180.0f / static_cast<f32>(M_PI)) * r;
}
constexpr f64 radiansToAngle(f64 r) { return (180.0 / M_PI) * r; }

template <typename T>
constexpr T scaleLinearAtAnchor(T value, T scale, T anchor) {
  return (value * scale) + (anchor - scale);
}

template <typename T> constexpr T lerp(T a, T b, f32 f) {
  return a + f * (b - a);
}

template <typename T> constexpr T clamp(T value, T min, T max) {
  return Max(Min(value, max), min);
}

/*
/ x = point on x axis
/ f = floor (min value)
/ r = roof (max value)
/ c = x offset
/ b = steepness
/
/ Graphing Calculator: https://www.desmos.com/calculator/gfcphg11cn
*/
extern f32 sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b);
extern f64 sigmoidCurve(f64 x, f64 f, f64 r, f64 c, f64 b);

namespace Vector3 {

inline f32 magnitude(const JGeometry::TVec3<f32> &vec) {
#if defined(SME_USE_PS_MATH)
  return PSVECMag(reinterpret_cast<const Vec *>(&vec));
#else
  return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
#endif
}

inline f32 magnitude(const Vec &vec) {
#if defined(SME_USE_PS_MATH)
  return PSVECMag(&vec);
#else
  return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
#endif
}

inline f32 getNormalAngle(const JGeometry::TVec3<f32> &vec) {
  return Math::radiansToAngle(atan2f(vec.x, vec.z));
}

inline f32 getNormalAngle(const Vec &vec) {
  return Math::radiansToAngle(atan2f(vec.x, vec.z));
}

inline JGeometry::TVec3<f32> normalized(const JGeometry::TVec3<f32> &vec) {
#if defined(SME_USE_PS_MATH)
  JGeometry::TVec3<f32> out;
  PSVECNormalize(reinterpret_cast<Vec *>(&out),
                 reinterpret_cast<const Vec *>(&vec));
  return out;
#else
  const f32 mag = magnitude(vec);
  return JGeometry::TVec3<f32>(vec.x / mag, vec.y / mag, vec.z / mag);
#endif
}

inline Vec normalized(const Vec &vec) {
#if defined(SME_USE_PS_MATH)
  JGeometry::TVec3<T> out;
  PSVECNormalize(reinterpret_cast<Vec *>(&out),
                 reinterpret_cast<const Vec *>(&vec));
  return out;
#else
  const f32 mag = magnitude(vec);
  return Vec{vec.x / mag, vec.y / mag, vec.z / mag};
#endif
}

inline f32 dot(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b) {
#if defined(SME_USE_PS_MATH)
  return PSVECCrossProduct(reinterpret_cast<const Vec *>(&a),
                           reinterpret_cast<const Vec *>(&b));
#else
  return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

inline f32 dot(const Vec &a, const Vec &b) {
#if defined(SME_USE_PS_MATH)
  return PSVECDotProduct(&a, &b);
#else
  return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

inline JGeometry::TVec3<f32> cross(const JGeometry::TVec3<f32> &a,
                                   const JGeometry::TVec3<f32> &b) {
#if defined(SME_USE_PS_MATH)
  Vec ret;
  PSVECCrossProduct(reinterpret_cast<const Vec *>(&a),
                    reinterpret_cast<const Vec *>(&b), &ret);
  return ret;
#else
  return JGeometry::TVec3<f32>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                               a.x * b.y - a.y * b.x);
#endif
}

inline Vec cross(const Vec &a, const Vec &b) {
#if defined(SME_USE_PS_MATH)
  Vec ret;
  PSVECCrossProduct(&a, &b, &ret);
  return ret;
#else
  return Vec{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x};
#endif
}

inline f32 lookAtRatio(const JGeometry::TVec3<f32> &a,
                       const JGeometry::TVec3<f32> &b) {
  f32 angle = atan2f(b.z, -b.x) - atan2f(a.z, a.x);
  if (angle > M_PI) {
    angle -= 2 * M_PI;
  } else if (angle <= -M_PI) {
    angle += 2 * M_PI;
  }
  return fabsf(angle) / M_PI;
}

inline f32 lookAtRatio(const Vec &a, const Vec &b) {
  f32 angle = atan2f(b.z, -b.x) - atan2f(a.z, a.x);
  if (angle > M_PI) {
    angle -= 2 * M_PI;
  } else if (angle <= -M_PI) {
    angle += 2 * M_PI;
  }
  return fabsf(angle) / M_PI;
}

} // namespace Vector3

} // namespace SME::Util::Math
