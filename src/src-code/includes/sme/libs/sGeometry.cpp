#include "sGeometry.hxx"

using namespace SME::Class;

TVectorTriangle::TVectorTriangle(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b, const JGeometry::TVec3<f32> &c)
{
    this->a = a;
    this->b = b;
    this->c = c;
};

f32 TVectorTriangle::bearingAngleY(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b)
{
    return SME::Util::Math::radiansToAngle(atan2f(b.x - a.x, b.z - a.z));
}

JGeometry::TVec3<f32> TVectorTriangle::cross(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b)
{
    JGeometry::TVec3<f32> x;
    PSVECCrossProduct(reinterpret_cast<const Vec *>(&a), reinterpret_cast<const Vec *>(&b), reinterpret_cast<Vec *>(&x));
    return x;
}

f32 TVectorTriangle::dot(const JGeometry::TVec3<f32> &a, const JGeometry::TVec3<f32> &b)
{
    return PSVECDotProduct(reinterpret_cast<const Vec *>(&a), reinterpret_cast<const Vec *>(&b));
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
        normal = TVectorTriangle::normalized(normal);

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

template <typename T> static T magnitude(const JGeometry::TVec3<T> &vec) {
    return (T)sqrtf((f32)(vec.x * vec.x) + (f32)(vec.y * vec.y) +
                    (f32)(vec.z * vec.z));
}

template <typename T> static T resultant(const JGeometry::TVec3<T> &vec) {
    return (T)(fabsf((f32)vec.x) + fabsf((f32)vec.y) + fabsf((f32)vec.z));
}

template <typename T> static T getNormalAngle(const JGeometry::TVec3<T> &vec) {
    return (T)((180.0f / M_PI) * atan2f(vec.x, vec.z));
}

template <typename T> static JGeometry::TVec3<T> unitVector(const JGeometry::TVec3<T> &vec) {
    T len = TVectorTriangle::resultant(vec);
    JGeometry::TVec3<T> unit(vec.x / len, vec.y / len, vec.z / len);
    return unit;
}

template <typename T> static JGeometry::TVec3<T> normalized(const JGeometry::TVec3<T> &vec) {
    T mag = TVectorTriangle::magnitude(vec);
    JGeometry::TVec3<T> unit(vec.x / mag, vec.y / mag, vec.z / mag);
    return unit;
}