#pragma once

#include "sms/JSystem/JGeometry.hxx"
#include "types.h"

namespace SME::Class {

struct TLightContext {
  enum class ActiveType : u8 { DISABLED, STATIC, FOLLOWPLAYER };

  TLightContext()
      : mShineShadowCoordinates(0.0f, 0.0f, 0.0f), mPrevShineCount(0),
        mPrevSize(0.0f), mNextSize(0.0f), mShineShadowBase(0.0f),
        mStepContext(0.0f),
        mLightType(ActiveType::DISABLED),
        mSizeMorphing(false) {}

  JGeometry::TVec3<f32> mShineShadowCoordinates;
  s32 mPrevShineCount;
  f32 mPrevSize;
  f32 mNextSize;
  f32 mShineShadowBase;
  f32 mStepContext;
  ActiveType mLightType;
  bool mSizeMorphing;
};


}