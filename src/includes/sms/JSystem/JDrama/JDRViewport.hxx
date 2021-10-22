#pragma once

#include "types.h"
#include "JDRNameRef.hxx"
#include "JDRViewObj.hxx"
#include "../JGeometry.hxx"

namespace JDrama {

class TViewport : public TViewObj {
public:
  TViewport(const TRect &, const char *);
  virtual ~TViewport();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  TRect mViewportRect; // 10
  u16 _20;
};

}