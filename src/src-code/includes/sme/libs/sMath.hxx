#include "math.h"

#ifndef SME_MATH_H
#define SME_MATH_H

#include "types.h"

#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Min(a, b) (((a) < (b)) ? (a) : (b))
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
inline T lerp(T a, T b, f32 f)
{
    return a + f * (b - a);
}

#endif