#pragma once

#include "math.h"
#include "MTX.h"

namespace JGeometry
{

    template <typename T>
    struct TVec2
    {
        TVec2();

        f32 dot(const TVec2<T> &) const;
        void sub(const TVec3<T> &);

        T x;
        T y;
    };

    template <typename T>
    struct TVec3
    {
        TVec3();
        TVec3(const TVec3<T> &);

        template <typename TY>
        TVec3(TY, TY, TY);
        
        TVec3<T> &operator =(const TVec3<T> &);
        TVec3<T> &operator *=(const TVec3<T> &);
        TVec3<T> &operator -=(const TVec3<T> &);
        
        void add(const TVec3<T> &);
        void div(f32);
        f32 dot(const TVec3<T> &) const;
        void negate();
        void scale(f32);
        void scale(f32, const TVec3<T> &);
        void scaleAdd(f32, const TVec3<T> &, const TVec3<T> &);
        void set(const Vec &);
        
        template <typename TY>
        void set(TY, TY, TY);

        template <typename TY>
        void set(const TVec3<TY> &);

        void setLength(const TVec3<T> &, f32);
        void setMax(const TVec3<T> &);
        void setMin(const TVec3<T> &);
        void sub(const TVec3<T> &);
        void sub(const TVec3<T> &, const TVec3<T> &);
        
        T x;
        T y;
        T z;
    };

    template <typename T>
    struct TVec4
    {
        TVec4();
        TVec4(const TVec4<T> &);
        
        TVec4<T> &operator =(const TVec4<T> &);
        
        f32 dot(const TVec4<T> &) const;
        void scale(f32, const TVec4<T> &);
        
        template <typename TY>
        void set(TY, TY, TY, TY);

        void xyz();

        T x;
        T y;
        T z;
        T w;
    };

    template <typename T>
    struct TQuat4
    {
        void rotate(const TVec3<T> &, const TVec3<T> &dst) const;
        void setRotate(const TVec3<T> &, const TVec3<T> &, f32);
        void slerp(const TQuat4<T> &);

        T x;
        T y;
        T z;
        T w;
    };

}; // namespace JGeometry