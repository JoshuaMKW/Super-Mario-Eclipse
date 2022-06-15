
#include "libs/sRand.hxx"
#include "Globals.hxx"
#include "sms/rand.h"

using namespace SME;

TVec3f Util::Rand::randUnitVector()
{
    float x = (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5;
    float y = (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5;
    float z = (static_cast<float>(rand()) / static_cast<float>(0x7fff)) - 0.5;

    float mag = sqrtf(x * x + y * y + z * z);
    return TVec3f(x / mag, y / mag, z / mag);
}

TVec3f Util::Rand::randVector(float scale)
{
    TVec3f randVec = randUnitVector();
    float r = (static_cast<float>(rand()) * scale / static_cast<float>(0x7fff));
    randVec.scale(r);
    return randVec;
}

float Util::Rand::randFromRange(f32 min, f32 max)
{
    float r = min + (static_cast<float>(rand()) / (static_cast<float>(0x7fff) / (max - min)));
    return r;
}