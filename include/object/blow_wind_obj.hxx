#pragma once

#include <Dolphin/types.h>
#include <JSystem/JDrama/JDRGraphics.hxx>
#include <SMS/MapObj/MapObjBase.hxx>
#include <SMS/MapObj/MapObjInit.hxx>


class TBlowWindMapObj : public TMapObjBase {
public:
  enum class GradientMode { CONSTANT, LINEAR, EXPONENTIAL, INVERSE };

  TBlowWindMapObj(const char *name);
  virtual ~TBlowWindMapObj();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  GradientMode getGradientMode() { return mMode; }
  void setGradientMode(GradientMode mode) { mMode = mode; }

  void lookAt(const TVec3f &position);

private:
  f32 mStrength;
  GradientMode mMode;
};

extern ObjData blowWindData;