#pragma once

#include "types.h"
#include "JDRPlacement.hxx"
#include "../JGeometry.hxx"
#include "../JStage/JSGActor.hxx"

namespace JDrama {

class TActor : public TPlacement, public JStage::TActor {
public:
  TActor(const char *);
  virtual ~TActor();

  virtual u32 getType() const override;
  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, TGraphics *) override;
  virtual void JSGGetTranslation(Vec *) const override;
  virtual void JSGSetTranslation(const Vec &) override;
  virtual void JSGGetScaling(Vec *) const override;
  virtual void JSGSetScaling(const Vec &) override;
  virtual void JSGGetRotation(Vec *) const override;
  virtual void JSGSetRotation(const Vec &) override;

  JGeometry::TVec3<f32> mSize;     // 24
  JGeometry::TVec3<f32> mRotation; // 30
private:
  u32 _02[0x8 / 4]; // 3C
};

}