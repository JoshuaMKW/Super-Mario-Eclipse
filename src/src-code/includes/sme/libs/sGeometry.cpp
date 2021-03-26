#include "sGeometry.hxx"

TVectorTriangle::TVectorTriangle(JGeometry::TVec3<f32> *a, JGeometry::TVec3<f32> *b, JGeometry::TVec3<f32> *c)
{
    this->a = *a;
    this->b = *b;
    this->c = *c;
};

TVectorTriangle &TVectorTriangle::operator=(TVectorTriangle &other)
{
    this->a = other.a;
    this->b = other.b;
    this->c = other.c;
    return *this;
}

f32 TVectorTriangle::bearingAngleY(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
{
    return radiansToAngle(atan2f(b.x - a.x, b.z - a.z));
}

JGeometry::TVec3<f32> TVectorTriangle::cross(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
{
    JGeometry::TVec3<f32> x;
    PSVECCrossProduct(reinterpret_cast<Vec *>(&a), reinterpret_cast<Vec *>(&b), reinterpret_cast<Vec *>(&x));
    return x;
}

f32 TVectorTriangle::dot(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b)
{
    return PSVECDotProduct(reinterpret_cast<Vec *>(&a), reinterpret_cast<Vec *>(&b));
}

JGeometry::TVec3<f32> TVectorTriangle::normal(bool normalize)
{
    JGeometry::TVec3<f32> vectorA(this->b.x - this->a.x,
                                  this->b.y - this->a.y,
                                  this->b.z - this->a.z);

    JGeometry::TVec3<f32> vectorB(this->c.x - this->a.x,
                                  this->c.y - this->a.y,
                                  this->c.z - this->a.z);

    JGeometry::TVec3<f32> normal = cross(vectorA, vectorB);
    if (normalize)
        normal = normal.normalized();

    return normal;
}

JGeometry::TVec3<f32> TVectorTriangle::center()
{
    return JGeometry::TVec3<f32>((a.x + b.x + c.x) / 3.0f,
                                 (a.y + b.y + c.y) / 3.0f,
                                 (a.z + b.z + c.z) / 3.0f);
}

f32 TVectorTriangle::yPosAtXZ(f32 x, f32 z)
{
    JGeometry::TVec3<float> normal = this->normal(false);
    return (-normal.x * (x - this->a.x) - normal.z * (z - this->a.z)) / normal.y - this->a.y;
}