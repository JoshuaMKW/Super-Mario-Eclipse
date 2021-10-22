#pragma once

#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "JStage/JSGCamera.hxx"
#include "JDRPlacement.hxx"
#include "types.h"

namespace JDrama {

class TCamera : public TPlacement, public JStage::TCamera {
public:
  virtual ~TCamera();

  virtual u32 getType() const;
  virtual u32 JSGGetFlag() const;
  virtual void JSGSetFlag(u32 flag);
  virtual f32 JSGGetProjectionNear() const;
  virtual void JSGSetProjectionNear(f32 projectionNear);
  virtual f32 JSGGetProjectionFar() const;
  virtual void JSGSetProjectionFar(f32 projectionFar);

private:
  u16 mFlag;           // 24
  f32 mProjectionNear; // 28
  f32 mProjectionFar;  // 2C
};

class TLookAtCamera : public TCamera {
public:
  virtual ~TLookAtCamera();

  virtual void perform(u32, TGraphics *);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 JSGGetProjectionFovy() const;
  virtual void JSGSetProjectionFovy(const f32 projectionFovy);
  virtual f32 JSGGetProjectionAspect() const;
  virtual void JSGSetProjectionAspect(const f32 projectionAspect);
  virtual void JSGGetViewPosition(Vec *) const;
  virtual void JSGSetViewPosition(const Vec &viewPos);
  virtual void JSGGetViewUpVector(Vec *) const;
  virtual void JSGSetViewUpVector(const Vec &upVector);
  virtual void JSGGetViewTargetPosition(Vec *) const;
  virtual void JSGSetViewTargetPosition(const Vec &targetPos);

private:
  JGeometry::TVec3<f32> mUpVector;  // 30
  JGeometry::TVec3<f32> mTargetPos; // 3C
  f32 mProjectionFovy;              // 48
  f32 mProjectionAspect;            // 4C
};

class TPolarCamera : public TCamera {
public:
  virtual ~TPolarCamera();

  virtual void perform(u32, TGraphics *);
  virtual void load(JSUMemoryInputStream &stream);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 JSGGetProjectionFovy() const;
  virtual void JSGSetProjectionFovy(f32 projectionFovy);
  virtual f32 JSGGetProjectionAspect() const;
  virtual void JSGSetProjectionAspect(f32 projectionAspect);

private:
  f32 mProjectionFovy;   // 30
  f32 mProjectionAspect; // 34
};

class TOrthoProj : public TCamera {
public:
  virtual ~TOrthoProj();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 *JSGGetProjectionField() const;
  virtual void JSGSetProjectionField(const f32 *projectionField);

private:
  JGeometry::TVec4<f32> mProjectionField; // 30
};

} // namespace JDrama