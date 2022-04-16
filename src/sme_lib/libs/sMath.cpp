#include "libs/sMath.hxx"
#include "SME.hxx"
#include "math.h"
#include "types.h"

using namespace SME::Util;

f32 SME_NO_INLINE Math::sigmoidCurve(f32 x, f32 f, f32 r, f32 c, f32 b) {
  return f + ((r - f) / (1.0f + expf((b * -1.0f) * (x - c))));
}

f64 SME_NO_INLINE Math::sigmoidCurve(f64 x, f64 f, f64 r, f64 c, f64 b) {
  return f + ((r - f) / (1.0f + expf((b * -1.0f) * (x - c))));
}

f32 Math::Vector3::magnitude(const TVec3f &vec) {
#if SME_USE_PS_MATH
  return PSVECMag(reinterpret_cast<const Vec *>(&vec));
#else
  return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
#endif
}

f32 Math::Vector3::magnitude(const Vec &vec) {
#if SME_USE_PS_MATH
  return PSVECMag(&vec);
#else
  return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
#endif
}

f32 Math::Vector3::getNormalAngle(const TVec3f &vec) {
  return Math::radiansToAngle(atan2f(vec.x, vec.z));
}

f32 Math::Vector3::getNormalAngle(const Vec &vec) {
  return Math::radiansToAngle(atan2f(vec.x, vec.z));
}

void Math::Vector3::normalized(const TVec3f &vec, TVec3f &out) {
#if SME_USE_PS_MATH
  PSVECNormalize(reinterpret_cast<const Vec *>(&vec),
                 reinterpret_cast<Vec *>(&out));
#else
  out.scale(magnitude(vec), vec);
#endif
}

void Math::Vector3::normalized(const Vec &vec, Vec &out) {
#if SME_USE_PS_MATH
  PSVECNormalize(&vec, &out);
#else
  const f32 mag = magnitude(vec);
  out.x = vec.x / mag;
  out.y = vec.y / mag;
  out.z = vec.z / mag;
#endif
}

f32 Math::Vector3::dot(const TVec3f &a, const TVec3f &b) {
#if SME_USE_PS_MATH
  return PSVECDotProduct(reinterpret_cast<const Vec *>(&a),
                         reinterpret_cast<const Vec *>(&b));
#else
  return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

f32 Math::Vector3::dot(const Vec &a, const Vec &b) {
#if SME_USE_PS_MATH
  return PSVECDotProduct(&a, &b);
#else
  return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

void Math::Vector3::cross(const TVec3f &a, const TVec3f &b, TVec3f &out) {
#if SME_USE_PS_MATH
  PSVECCrossProduct(reinterpret_cast<const Vec *>(&a),
                    reinterpret_cast<const Vec *>(&b),
                    reinterpret_cast<Vec *>(&out));
#else
  out.x = a.y * b.z - a.z * b.y;
  out.y = a.z * b.x - a.x * b.z;
  out.z = a.x * b.y - a.y * b.x;
#endif
}

void Math::Vector3::cross(const Vec &a, const Vec &b, Vec &out) {
#if SME_USE_PS_MATH
  PSVECCrossProduct(&a, &b, &out);
#else
  out.x = a.y * b.z - a.z * b.y;
  out.y = a.z * b.x - a.x * b.z;
  out.z = a.x * b.y - a.y * b.x;
#endif
}

f32 Math::Vector3::getYAngleTo(const TVec3f &a, const TVec3f &b) {
  TVec3f diff = a;
  diff.sub(b);
  return Math::radiansToAngle(atan2f(diff.z, diff.x));
}

f32 Math::Vector3::lookAtRatio(const TVec3f &a, const TVec3f &b) {
  f32 angle = atan2f(b.z, -b.x) - atan2f(a.z, a.x);
  if (angle > M_PI) {
    angle -= 2 * M_PI;
  } else if (angle <= -M_PI) {
    angle += 2 * M_PI;
  }
  return fabsf(angle) / M_PI;
}

f32 Math::Vector3::lookAtRatio(const Vec &a, const Vec &b) {
  f32 angle = atan2f(b.z, -b.x) - atan2f(a.z, a.x);
  if (angle > M_PI) {
    angle -= 2 * M_PI;
  } else if (angle <= -M_PI) {
    angle += 2 * M_PI;
  }
  return fabsf(angle) / M_PI;
}

f32 Math::Vector3::angleBetween(const TVec3f &a, const TVec3f &b) {
  return dot(a, b) / (magnitude(a) * magnitude(b));
}

f32 Math::Vector3::angleBetween(const Vec &a, const Vec &b) {
  return dot(a, b) / (magnitude(a) * magnitude(b));
}