#pragma once

#include "JSGObject.hxx"
#include "types.h"
#include "MTX.h"

namespace JStage {

class TActor : public TObject {
public:
  virtual ~TActor();

  virtual unsigned long JSGFGetType() const;
  virtual void JSGGetTranslation(Vec *) const;
  virtual void JSGSetTranslation(const Vec &);
  virtual void JSGGetScaling(Vec *) const;
  virtual void JSGSetScaling(const Vec &);
  virtual void JSGGetRotation(Vec *) const;
  virtual void JSGSetRotation(const Vec &);

  virtual unsigned long JSGGetShape() const;
  virtual void JSGSetShape(unsigned long);
  virtual unsigned long JSGGetAnimation() const;
  virtual void JSGSetAnimation(unsigned long);
  virtual f32 JSGGetAnimationFrame() const;
  virtual void JSGSetAnimationFrame(f32);
  virtual f32 JSGGetAnimationFrameMax() const;
};

} // namespace JStage