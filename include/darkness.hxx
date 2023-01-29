#pragma once

#include "JGeometry.hxx"
#include "types.h"

namespace SME::Class {

struct TLightContext {
  enum class ActiveType : u8 { DISABLED, STATIC, FOLLOWPLAYER };

  TLightContext()
      : mShineShadowCoordinates(0.0f, 0.0f, 0.0f), mPrevShineCount(0),
        mPrevSize(0.0f), mNextSize(0.0f), mShineShadowBase(0.0f),
        mStepContext(0.0f), mPrevDarkness(0), mNextDarkness(0),
        mLightType(ActiveType::DISABLED),
        mSizeMorphing(false) {}

  TVec3f mShineShadowCoordinates;
  s32 mPrevShineCount;
  f32 mPrevSize;
  f32 mNextSize;
  f32 mShineShadowBase;
  f32 mStepContext;
  u8 mPrevDarkness;
  u8 mNextDarkness;
  ActiveType mLightType;
  bool mSizeMorphing;
};


}