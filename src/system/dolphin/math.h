#ifndef MATH_H
#define MATH_H

#include "types.h"

#define M_PI 3.14159265358979323846
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef __cplusplus
extern "C"
{
#endif

    f32 acosf(f32);
    f64 atan(f64);
    f32 atanf(f32);
    f64 atan2(f64, f64);
    f32 atan2f(f32, f32);
    f64 cos(f64 rad);
    f32 cosf(f32 rad);
    f32 expf(f32 x);
    f64 fabs(f64 x);
    f32 fabsf(f32 x);
    f64 frexp(f64, int *);
    f32 powf(f32 x, f32 power);
    f64 sin(f64);
    f32 sinf(f32 rad);
    f32 tanf(f32 rad);
    f64 sqrt(f64 x);
    f32 sqrtf(f32 x);

    extern f32 NAN;

#ifdef __cplusplus
}
#endif


#endif