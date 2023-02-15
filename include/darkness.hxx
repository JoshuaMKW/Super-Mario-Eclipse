#pragma once

#include <Dolphin/types.h>
#include <JSystem/JGeometry/JGMVec.hxx>

#include <SMS/Manager/ModelWaterManager.hxx>

struct TLightContext {
  enum class ActiveType : u8 { DISABLED, STATIC, FOLLOWPLAYER };

  TLightContext()
      : mLightType(ActiveType::DISABLED), mTranslation(0.0f, 0.0f, 0.0f), mColor(0, 0, 10, 255),
        mLayerCount(24), mBaseScale(1.0f), mPrevSize(0.0f), mNextSize(0.0f), mStepContext(0.0f),
        mPrevDarkness(0), mNextDarkness(0), mTargetDarkness(0), mPrevShineCount(0), mSizeMorphing(false) {}

  void process(TModelWaterManager &manager);

  ActiveType mLightType;
  TVec3f mTranslation;
  JUtility::TColor mColor;
  u8 mLayerCount;
  f32 mBaseScale;
  f32 mPrevSize;
  f32 mNextSize;
  f32 mStepContext;
  u8 mPrevDarkness;
  u8 mNextDarkness;
  u8 mTargetDarkness;
  s32 mPrevShineCount;
  bool mSizeMorphing;
};

void initializeParameters(TLightContext::ActiveType type, TVec3f pos, u8 layer_count,
                          JUtility::TColor color, f32 scale, u8 brightness);