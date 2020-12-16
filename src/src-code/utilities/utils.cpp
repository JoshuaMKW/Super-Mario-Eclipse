#include "../includes/eclipse.hxx"

float angleToRadians(float angle)
{
    return (PI / 180.0f) * angle;
}

float radiansToAngle(float rad)
{
    return (180.0f / PI) * rad;
}

JGeometry::TVec3<float> getTriCenter(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b, JGeometry::TVec3<float> c)
{
    JGeometry::TVec3<float> center((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f, (a.z + b.z + c.z) / 3.0f);
    return center;
}

u32 *branchToAddr(u32 *bAddr)
{
    s32 offset;
    if (*bAddr & 0x2000000)
        offset = (*bAddr & 0x3FFFFFD) - 0x4000000;
    else
        offset = *bAddr & 0x3FFFFFD;
    return (u32 *)((u32)bAddr + offset);
}

/*
/ r = roof (max value)
/ b = steepness
/ c = x offset
/ f = floor (min value)
/
/ Graphing Calculator: https://www.desmos.com/calculator/kn9tpwdan5
*/
float sigmoidCurve(float x, float f, float r, float c, float b)
{
    float roof = fabsf(r - f);
    return f + (roof / (1.0f + expf((b * -1.0f) * (x - c))));
}

bool isDigit(char *chr)
{
    return *chr >= '0' && *chr <= '9';
}

bool isDigit(char chr)
{
    return chr >= '0' && chr <= '9';
}

inline void flushAddr(void *addr)
{
    dcbf(addr);
    icbi(addr);
}