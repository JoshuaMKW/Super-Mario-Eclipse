#include "WarpCollision.hxx"
#include "libs/sGeometry.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "libs/sMath.hxx"


using namespace SME;
using namespace SME::Util::Math;

Class::TWarpCollisionList::TWarpCollisionList(Class::TCollisionLink colList[]) {
  u8 size = 0;
  for (u32 i = 0; &colList[i] != nullptr; ++i) {
    mColList[i] = colList[i];
    size += 1;
  }
  mArrayLength = size;
}

TBGCheckData *
Class::TWarpCollisionList::resolveCollisionWarp(TBGCheckData *colTriangle) {
  if ((u8)(colTriangle->mValue4 >> 8) == 0xFF)
    return nullptr;

  return getNearestTarget(colTriangle);
}

TBGCheckData *
Class::TWarpCollisionList::getNearestTarget(TBGCheckData *colTriangle) {
  TVectorTriangle colVector;
  TVectorTriangle targetVector;

  colVector.a = colTriangle->mVertexA;
  colVector.b = colTriangle->mVertexB;
  colVector.c = colTriangle->mVertexC;

  TCollisionLink members[0xFF];
  f32 nearestDist = 10000000000.0f;
  s32 index = 0;

  for (u32 i = 0; i < mArrayLength; ++i) {
    if (mColList[i].mThisID == 0xFF)
      continue;
    if (mColList[i].mThisID == (u8)(colTriangle->mValue4 >> 8) &&
        mColList[i].mColTriangle != colTriangle) {
      members[index] = mColList[i];
      index++;
    }
  }

  JGeometry::TVec3<f32> a;
  JGeometry::TVec3<f32> b;
  index = -1;

  for (u32 i = 0; i <= index; ++i) {
    targetVector.a = members[i].mColTriangle->mVertexA;
    targetVector.b = members[i].mColTriangle->mVertexB;
    targetVector.c = members[i].mColTriangle->mVertexC;

    JGeometry::TVec3<f32> a(colVector.center());
    JGeometry::TVec3<f32> b(targetVector.center());

    JGeometry::TVec3<f32> _set(b.x - a.x, b.y - a.y, b.z - a.z);

    if (Vector::magnitude(_set) < nearestDist) {
      nearestDist = Vector::magnitude(_set);
      index = i;
    }
  }
  if (index == -1)
    return nullptr;

  return members[index].mColTriangle;
}