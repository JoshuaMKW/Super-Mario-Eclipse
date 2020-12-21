#ifndef JGEOMETRY_H
#define JGEOMETRY_H

#include "math.h"
#include "MTX.h"

namespace JGeometry
{

    template <typename T>
    struct TVec2
    {
        T x;
        T y;

        TVec2() {}

        TVec2(T x, T y)
        {
            this->x = x;
            this->y = y;
        }

        TVec2(TVec2 *v)
        {
            this->x = v->x;
            this->y = v->y;
        }

        void operator =(TVec2<T> &other)
        {
            this->x = other.x;
            this->y = other.y;
        }

        T magnitude()
        {
            return (T)sqrtf((f32)(this->x * this->x) + (f32)(this->y * this->y));
        }

        T resultant()
        {
            return (T)(fabsf((f32)this->x) + fabsf((f32)this->y));
        }

        TVec2<T> unitVector()
        {
            T len = this->resultant();
            TVec2<T> unit(this->x / len, this->y / len);
            return unit;
        }

        TVec2<T> normalized()
        {
            T mag = this->magnitude();
            TVec2<T> unit(this->x / mag, this->y / mag);
            return unit;
        }
    };

    template <typename T>
    struct TVec3
    {
        T x;
        T y;
        T z;

        TVec3() {}

        TVec3(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        TVec3(TVec3<T> *v)
        {
            this->x = v->x;
            this->y = v->y;
            this->z = v->z;
        }

        void operator =(TVec3<T> &other)
        {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
        }

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

        TVec3<f32> psNormalized()
        {
            TVec3<f32> n;
            PSVECNormalize((Vec *)&this, (Vec *)&n);
            return n;
        }

        TVec3<f32> psScaled(f32 s)
        {
            TVec3<f32> scaled;
            PSVECScale((Vec *)&this, (Vec *)&scaled, s);
            return scaled;
        }

        TVec3<T> normalized()
        {
            T mag = this->magnitude();
            TVec3<T> unit(this->x / mag, this->y / mag, this->z / mag);
            return unit;
        }

        TVec3<T> scaled(f32 s)
        {
            JGeometry::TVec3<T> scl(this->x * s, this->y * s, this->z * s);
            return scl;
        }
    };

    template <typename T>
    struct TVec4
    {
        T x;
        T y;
        T z;
        T w;

        TVec4() {}

        TVec4(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        TVec4(TVec4<T> *v)
        {
            this->x = v->x;
            this->y = v->y;
            this->z = v->z;
            this->w = v->w;
        }

        void operator =(TVec4<T> &other)
        {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
            this->w = other.w;
        }

        T magnitude()
        {
            return (T)sqrtf((f32)(this->x * this->x) + (f32)(this->y * this->y) + (f32)(this->z * this->z) + (f32)(this->w * this->w));
        }

        T resultant()
        {
            return (T)(fabsf((f32)this->x) + fabsf((f32)this->y) + fabsf((f32)this->z) + fabsf((f32)this->w));
        }

        TVec4<T> unitVector()
        {
            T len = this->resultant();
            TVec4<T> unit(this->x / len, this->y / len, this->z / len, this->w / len);
            return unit;
        }

        TVec4<T> normalized()
        {
            T mag = this->magnitude();
            TVec4<T> unit(this->x / mag, this->y / mag, this->z / mag, this->w / mag);
            return unit;
        }
    };

    template <typename T>
    struct TQuat4
    {
        T x;
        T y;
        T z;
        T w;

        TQuat4() {}

        TQuat4(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        TQuat4(TQuat4<T> *v)
        {
            this->x = v->x;
            this->y = v->y;
            this->z = v->z;
            this->w = v->w;
        }

        void operator =(JGeometry::TQuat4<T> &other)
        {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
            this->w = other.w;
        }
    };

}; // namespace JGeometry

#endif