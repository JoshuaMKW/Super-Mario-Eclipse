#pragma once

#include "MTX.h"
#include "math.h"
#include "equivtype.hxx"

namespace JGeometry {

template <typename T> struct TVec2 {
  TVec2();

  f32 dot(const TVec2 &) const;
  void sub(const TVec2 &);

  T x;
  T y;
};

template <typename T> struct TVec3 {
  T x;
  T y;
  T z;

  TVec3();
  TVec3(const TVec3 &);
  template <typename TY> TVec3(TY, TY, TY);

  operator Vec *() const {
    static_assert(is_equal_type<T, f32>());
    return (Vec *)&x;
  }
  operator const Vec *() const {
    static_assert(is_equal_type<T, f32>());
    return (const Vec *)&x;
  }

  // Because PAL is missing this operator
  TVec3 &operator=(const TVec3 &other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
  };
  TVec3 &operator*=(const TVec3 &);
  TVec3 &operator-=(const TVec3 &);

  TVec3 operator+(const TVec3 &other) {
    return {x + other.x, y + other.y, z + other.z};
  }

  TVec3 operator-(const TVec3 &other) {
    return {x - other.x, y - other.y, z - other.z};
  }

  void set(const TVec3 &other) {
    x = other.x;
    y = other.y;
    z = other.z;
  }
  void set(const Vec &other) {
    x = other.x;
    y = other.y;
    z = other.z;
  }
  void set(f32 _x, f32 _y, f32 _z) {
    x = _x;
    y = _y;
    z = _z;
  }

  void add(const TVec3 &);
  void div(f32);
  f32 dot(const TVec3 &) const;
  void negate();
  void scale(f32);
  void scale(f32, const TVec3 &);
  void scaleAdd(f32, const TVec3 &, const TVec3 &);

  void setLength(const TVec3 &, f32);
  void setMax(const TVec3 &);
  void setMin(const TVec3 &);
  void sub(const TVec3 &);
  void sub(const TVec3 &, const TVec3 &);

  static inline TVec3 up() {
    return TVec3<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
  }
  static inline TVec3 down() {
    return TVec3<T>(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0));
  }
  static inline TVec3 forward() {
    return TVec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
  }
  static inline TVec3 backward() {
    return TVec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1));
  }
  static inline TVec3 right() {
    return TVec3<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
  }
  static inline TVec3 left() {
    return TVec3<T>(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(-1));
  }
};

template <typename T> struct TVec4 {
  TVec4();
  TVec4(const TVec4 &);

  TVec4 &operator=(const TVec4 &);

  f32 dot(const TVec4 &) const;
  void scale(f32, const TVec4 &);

  template <typename TY> void set(TY, TY, TY, TY);

  void xyz();

  T x;
  T y;
  T z;
  T w;
};

template <typename T> struct TQuat4 {
  void rotate(const TVec3<T> &axis, TVec3<T> &dst) const;
  void setRotate(const TVec3<T> &axis, const TVec3<T> &dst, f32 rotation);
  void slerp(const TQuat4 &target, f32 time);

  T x;
  T y;
  T z;
  T w;
};

}; // namespace JGeometry

typedef JGeometry::TVec2<f32> TVec2f;
typedef JGeometry::TVec3<f32> TVec3f;
typedef JGeometry::TVec3<s16> TVec3s;
typedef JGeometry::TVec4<f32> TVec4f;