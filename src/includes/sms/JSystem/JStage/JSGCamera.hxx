#pragma once

#include "JSGObject.hxx"
#include "types.h"

namespace JStage {

enum TECameraProjection {};

enum TECameraView {};

class TCamera : public TObject {
public:
  virtual ~TCamera();

  virtual s32 JSGFGetType() const;
  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(TECameraProjection);
  virtual f32 JSGGetProjectionNear() const;
  virtual void JSGSetProjectionNear(f32);
  virtual f32 JSGGetProjectionFar() const;
  virtual void JSGSetProjectionFar(f32);
  virtual f32 JSGGetProjectionFovy() const;
  virtual void JSGSetProjectionFovy(f32);
  virtual f32 JSGGetProjectionAspect() const;
  virtual void JSGSetProjectionAspect(f32);
  virtual f32 *JSGGetProjectionField() const;
  virtual void JSGSetProjectionField(f32 const *);
  virtual s32 JSGGetViewType() const;
  virtual void JSGSetViewType(TECameraView);
};

} // namespace JStage