#pragma once

#include "types.h"
#include "MTX.h"
#include "sms/JSystem/JGeometry.hxx"

#include "sMath.hxx"

class TVectorTriangle
{
public:
    JGeometry::TVec3<f32> a;
    JGeometry::TVec3<f32> b;
    JGeometry::TVec3<f32> c;

    TVectorTriangle() {};
    TVectorTriangle(JGeometry::TVec3<f32> *a, JGeometry::TVec3<f32> *b, JGeometry::TVec3<f32> *c);

    TVectorTriangle &operator =(TVectorTriangle &other);

    static f32 bearingAngleY(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    static JGeometry::TVec3<f32> cross(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    static f32 dot(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    
    JGeometry::TVec3<f32> center();
    JGeometry::TVec3<f32> normal(bool normalize = true);
    f32 yPosAtXZ(f32 x, f32 z);
};

/*
T magnitude()
{
    return (T)sqrtf((f32)(this->x * this->x) + (f32)(this->y * this->y) + (f32)(this->z * this->z));
}

T resultant()
{
    return (T)(fabsf((f32)this->x) + fabsf((f32)this->y) + fabsf((f32)this->z));
}

T getNormalAngle()
{
    return (T)((180.0f / PI) * atan2f(this->x, this->z));
}

TVec3<T> unitVector()
{
    T len = this->resultant();
    TVec3<T> unit(this->x / len, this->y / len, this->z / len);
    return unit;
}

TVec3<T> normalized()
{
    T mag = this->magnitude();
    TVec3<T> unit(this->x / mag, this->y / mag, this->z / mag);
    return unit;
}
*/