#pragma once

#include "types.h"
#include "JGeometry.hxx"

namespace SME::Util::Rand
{
TVec3f randUnitVector();
TVec3f randVector(f32 scale);
float randFromRange(f32 min,f32 max);
}