#include "libs/sGeometry.hxx"

using namespace SME::Class;
using namespace SME::Util::Math;

TVectorTriangle::TVectorTriangle(const JGeometry::TVec3<f32> &_a,
                                 const JGeometry::TVec3<f32> &_b,
                                 const JGeometry::TVec3<f32> &_c) {
  a = _a;
  b = _b;
  c = _c;
};

f32 TVectorTriangle::bearingAngleY(const JGeometry::TVec3<f32> &a,
                                   const JGeometry::TVec3<f32> &b) {
  return radiansToAngle(atan2f(b.x - a.x, b.z - a.z));
}

void TVectorTriangle::normal(bool normalize, JGeometry::TVec3<f32> &out) {
  JGeometry::TVec3<f32> vectorA(b.x - a.x, b.y - a.y, b.z - a.z);

  JGeometry::TVec3<f32> vectorB(c.x - a.x, c.y - a.y, c.z - a.z);

  JGeometry::TVec3<f32> vec;
  Vector3::cross(vectorA, vectorB, vec);
  if (normalize)
    Vector3::normalized(vec, out);
}

void TVectorTriangle::center(JGeometry::TVec3<f32> &out) {
  out.set((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f,
          (a.z + b.z + c.z) / 3.0f);
}

f32 TVectorTriangle::yPosAtXZ(f32 x, f32 z) {
  JGeometry::TVec3<f32> n;
  normal(false, n);
  return (-n.x * (x - a.x) - n.z * (z - a.z)) / n.y - a.y;
}