#pragma once

#include "types.h"
#include "sms/GC2D/Coord2D.hxx"
#include "J2D/J2DPane.hxx"
#include "JUT/JUTRect.hxx"

class TExPane {
public:
  void setPanePosition(s32, const JUTPoint &begin, const JUTPoint &mid,
                       const JUTPoint &end);
  void setPaneSize(s32, const JUTPoint &begin, const JUTPoint &mid,
                   const JUTPoint &end);
  void update();

  J2DPane *mPane;
  JUTRect mRect;
  TCoord2D mOffset;
  TCoord2D mSize;
  f32 mCurrentPoint;
  f32 mIncPoint;
  s16 _4C;
  bool mIsOffsetUpdating;
  bool mIsSizeUpdating;
  bool mIsAlphaUpdating;
};