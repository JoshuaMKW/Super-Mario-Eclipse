#pragma once

#include "types.h"
#include "GX.h"

#include "JDRVideo.hxx"

namespace JDrama {

class TDisplay {
public:
  TDisplay(u16 retraceCount, void *, void *, const GXRenderModeObj &);

  void startRendering();
  void endRendering();

  u32 _00[4];
  GXRenderModeObj mRenderObj;
  u16 mRetraceCount;
  u16 _4E;
  u32 _50[0x10 / 4];
  JDrama::TVideo *mVideo; // 0x0060
  u16 _64;
};

}