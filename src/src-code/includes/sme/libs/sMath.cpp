#include "types.h"
#include "math.h"
#include "sMath.hxx"

using namespace SME::Util;

f32 Math::sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b)
{
    f32 roof = r - f;
    return f + (roof / (1.0f + expf((b * -1.0f) * (x - c))));
}

f64 Math::sigmoidCurve(f64 x, f64 f, f64 r, f64 c, f64 b)
{
    f64 roof = r - f;
    return f + (roof / (1.0f + expf((b * -1.0f) * (x - c))));
}