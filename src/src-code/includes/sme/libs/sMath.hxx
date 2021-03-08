#include "math.h"

#pragma once

#include "types.h"

#define AngleToRadians(a) (PI / 180.0f) * (a)
#define RadiansToAngle(r) (180.0f / PI) * (r)

/*
/ r = roof (max value)
/ b = steepness
/ c = x offset
/ f = floor (min value)
/
/ Graphing Calculator: https://www.desmos.com/calculator/kn9tpwdan5
*/
extern f32 sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b);

template <typename T>
constexpr inline T lerp(T a, T b, f32 f) { return a + f * (b - a); }