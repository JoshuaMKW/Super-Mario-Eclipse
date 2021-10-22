#pragma once

#include "types.h"
#include "JUT/JUTPoint.hxx"
#include "J2D/J2DPane.hxx"
#include "JUT/JUTRect.hxx"

class TBoundPane {
public:
  void setPanePosition(s32, const JUTPoint &begin, const JUTPoint &mid,
                       const JUTPoint &end);
  void setPaneSize(s32, const JUTPoint &begin, const JUTPoint &mid,
                   const JUTPoint &end);
  void update();

  J2DPane *mPane;
  JUTRect mRect;
  JUTRect _14;
  bool mActive;            // 0x24
  f32 mCurrentInterpolate; // 0x28
  f32 mIncInterpolate;     // 0x2C
  f32 _30;
  f32 _34;
  JUTPoint mStartPos; // 0x38
  JUTPoint mMidPos;   // 0x40
  JUTPoint mEndPos;   // 0x48
  u32 _50[0x18 / 4];
};