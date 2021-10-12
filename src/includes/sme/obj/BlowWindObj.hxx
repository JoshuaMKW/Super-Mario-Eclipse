#pragma once

#include "sms/JSystem/JDrama.hxx"
#include "sms/mapobj/MapObjBase.hxx"

class TBlowWindObj : public TMapObjBase {
public:
  enum class GradientMode { CONSTANT, LINEAR, EXPONENTIAL, INVERSE };

  TBlowWindObj(const char *name);
  virtual ~TBlowWindObj();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  GradientMode getGradientMode() { return mMode; }
  void setGradientMode(GradientMode mode) { mMode = mode; }

  void lookAt(const JGeometry::TVec3<f32> &position);

private:
  f32 mStrength;
  GradientMode mMode;
};