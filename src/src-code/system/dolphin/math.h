#ifndef MATH_H
#define MATH_H

#include "types.h"

#define PI 3.14159265f

#ifdef __cplusplus
extern "C"
{
#endif

    double atan(double);
    double atan2(double, double);
    double cos(double);
    double fabs(double x);
    double frexp(double, int *);
    double sin(double);

    float acosf(float);
    float atanf(float);
    float atan2f(float, float);
    float cosf(float);
    float expf(float);
    float fabsf(float x);
    float powf(float, float);
    float sinf(float);
    inline float sqrtf(float x) { return powf(x, 0.5); };
    float tanf(float);

    extern float NAN;

#ifdef __cplusplus
}
#endif

#endif