#pragma once

#include "JDRRect.hxx"
#include "types.h"

namespace JDrama {

class TGraphics {
public:
  void setViewport(const TRect &, f32, f32);

  u8 _0[0x20];
  bool _20;
  u8 _21[0x54 - 0x21];
  TRect mViewPortSpace;
};

} // namespace JDrama