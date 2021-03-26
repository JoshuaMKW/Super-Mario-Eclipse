#include "math.h"

#pragma once

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

/*
/ x = point on x axis
/ f = floor (min value)
/ r = roof (max value)
/ c = x offset
/ b = steepness
/
/ Graphing Calculator: https://www.desmos.com/calculator/kn9tpwdan5
*/
constexpr f32 sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b) {
  return f + ((r - f) / (1.0f + expf((b * -1.0f) * (x - c))));
}

constexpr f64 sigmoidCurve(f64 x, f64 f, f64 r, f64 c, f64 b) {
  return f + ((r - f) / (1.0f + expf((b * -1.0f) * (x - c))));
}

template <typename T> constexpr inline T lerp(T a, T b, f32 f) {
  return a + f * (b - a);
}

} // namespace SME::Util::Math
