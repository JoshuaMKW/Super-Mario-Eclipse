#include "types.h"
#include "math.h"
#include "sMath.hxx"

using namespace SME::Util::Math;

template <typename T> T Vector::magnitude(const JGeometry::TVec3<T> &vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

template <typename T> T Vector::getNormalAngle(const JGeometry::TVec3<T> &vec)
{
    return (T)((180.0f / M_PI) * atan2f(this->x, this->z));
}

template <typename T>
JGeometry::TVec3<T> Vector::normalized(const JGeometry::TVec3<T> &vec)
{
    f32 mag = Vector::magnitude(vec);
    return JGeometry::TVec3<T>(vec.x / mag, vec.y / mag, vec.z / mag);
}