#pragma once

#include "../J3D/J3DDrawBuffer.hxx"
#include "../JGeometry.hxx"
#include "JDRPlacement.hxx"
#include "types.h"

namespace JDrama {

class TDrawBufObj : public TViewObj {
public:
  TDrawBufObj();
  TDrawBufObj(u32, u32, const char *);
  virtual ~TDrawBufObj();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  J3DDrawBuffer *mDrawBuffer; // 10
  u32 mBufferSize;            // 14
  u32 _18;                    // init'd to 7
};

} // namespace JDrama