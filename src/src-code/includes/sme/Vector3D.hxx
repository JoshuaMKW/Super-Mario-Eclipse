#ifndef SME_VEC_H
#define SME_VEC_H

#include "types.h"
#include "MTX.h"
#include "sms/JGeometry.hxx"

#include "libs/sMath.hxx"

class Vector3D
{
public:
    JGeometry::TVec3<f32> a;
    JGeometry::TVec3<f32> b;
    JGeometry::TVec3<f32> c;

    Vector3D() {};
    Vector3D(JGeometry::TVec3<f32> *a, JGeometry::TVec3<f32> *b, JGeometry::TVec3<f32> *c)
    {
        this->a = *a;
        this->b = *b;
        this->c = *c;
    };

    void operator =(Vector3D &other)
    {
        this->a = other.a;
        this->b = other.b;
        this->c = other.c;
    }

    static f32 bearingAngleY(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
    {
        return RadiansToAngle(atan2f(b.x - a.x, b.z - a.z));
    }

    static JGeometry::TVec3<f32> cross(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
    {
        JGeometry::TVec3<f32> x;
        PSVECCrossProduct((Vec *)&a, (Vec *)&b, (Vec *)&x);
        return x;
    }

    static f32 dot(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
    {
        return PSVECDotProduct((Vec *)&a, (Vec *)&b);
    }

    JGeometry::TVec3<f32> normal()
    {
        JGeometry::TVec3<f32> vectorA(this->b.x - this->a.x,
                                      this->b.y - this->a.y,
                                      this->b.z - this->a.z);

        JGeometry::TVec3<f32> vectorB(this->c.x - this->a.x,
                                      this->c.y - this->a.y,
                                      this->c.z - this->a.z);

        JGeometry::TVec3<f32> normal = cross(vectorA, vectorB).normalized();
        return normal;
    }

    JGeometry::TVec3<f32> center()
    {
        JGeometry::TVec3<f32> center((a.x + b.x + c.x) / 3.0f,
                                     (a.y + b.y + c.y) / 3.0f,
                                     (a.z + b.z + c.z) / 3.0f);
        return center;
    }
};

#endif