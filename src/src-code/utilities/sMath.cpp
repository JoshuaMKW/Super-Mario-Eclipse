#include "types.h"
#include "math.h"

f32 sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b)
{
    float roof = fabsf(r - f);
    return f + (roof / (1.0f + expf((b * -1.0f) * (x - c))));
}