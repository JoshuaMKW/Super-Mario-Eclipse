#pragma once

#include "math.h"
#include "MTX.h"

namespace JGeometry
{

    template <typename T>
    struct TVec2
    {
        TVec2();

        f32 dot(const TVec2 &) const;
        void sub(const TVec2 &);

        T x;
        T y;
    };

    template <typename T>
    struct TVec3
    {
        TVec3();
        TVec3(const TVec3 &);

        template <typename TY>
        TVec3(TY, TY, TY);
        
        TVec3 &operator =(const TVec3 &);
        TVec3 &operator *=(const TVec3 &);
        TVec3 &operator -=(const TVec3 &);
        
        void add(const TVec3 &);
        void div(f32);
        f32 dot(const TVec3 &) const;
        void negate();
        void scale(f32);
        void scale(f32, const TVec3 &);
        void scaleAdd(f32, const TVec3 &, const TVec3 &);
        void set(const Vec &);
        
        template <typename TY>
        void set(TY, TY, TY);

        template <typename TY>
        void set(const TVec3<TY> &);

        void setLength(const TVec3 &, f32);
        void setMax(const TVec3 &);
        void setMin(const TVec3 &);
        void sub(const TVec3 &);
        void sub(const TVec3 &, const TVec3 &);
        
        T x;
        T y;
        T z;
    };

    template <typename T>
    struct TVec4
    {
        TVec4();
        TVec4(const TVec4 &);
        
        TVec4 &operator =(const TVec4 &);
        
        f32 dot(const TVec4 &) const;
        void scale(f32, const TVec4 &);
        
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
        void slerp(const TQuat4 &);

        T x;
        T y;
        T z;
        T w;
    };

}; // namespace JGeometry