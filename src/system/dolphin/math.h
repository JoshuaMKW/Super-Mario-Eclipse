#ifndef MATH_H
#define MATH_H

#include "PPCArch.h"
#include "types.h"

#define M_PI 3.14159265358979323846
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef __cplusplus
extern "C"
{
#endif

    f64 atan(f64);
    f64 atan2(f64, f64);
    f64 cos(f64);
    f64 fabs(f64 x);
    f64 frexp(f64, int *);
    f64 sin(f64);

    f32 acosf(f32);
    f32 atanf(f32);
    f32 atan2f(f32, f32);
    f32 cosf(f32);
    f32 expf(f32);
    f32 fabsf(f32 x);
    f32 powf(f32, f32);
    f32 sinf(f32);
    f32 tanf(f32);
    f64 sqrt(f64 d);
    f32 sqrtf(f32 d);

    extern f32 NAN;

#ifdef __cplusplus
}
#endif


#endif