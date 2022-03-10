#include "math.h"

#pragma once

#include "JGeometry.hxx"
#include "macros.h"
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

f32 magnitude(const JGeometry::TVec3<f32> &vec);
f32 magnitude(const Vec &vec);

f32 getNormalAngle(const JGeometry::TVec3<f32> &vec);
f32 getNormalAngle(const Vec &vec);

void normalized(const JGeometry::TVec3<f32> &vec, JGeometry::TVec3<f32> &out);
void normalized(const Vec &vec, Vec &out);

f32 dot(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b);
f32 dot(const Vec &a, const Vec &b);

void cross(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b,
           JGeometry::TVec3<f32> &out);
void cross(const Vec &a, const Vec &b, Vec &out);

f32 lookAtRatio(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b);
f32 lookAtRatio(const Vec &a, const Vec &b);

f32 angleBetween(const JGeometry::TVec3<f32> &a,
                 const JGeometry::TVec3<f32> &b);
f32 angleBetween(const Vec &a, const Vec &b);

} // namespace Vector3

} // namespace SME::Util::Math
