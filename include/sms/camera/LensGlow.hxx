#pragma once

#include "J3D/J3DAnimation.hxx"
#include "JDrama/JDRGraphics.hxx"

class TLensGlow : public JDrama::TViewObj {
public:
  TLensGlow(bool, const char *name);
  virtual ~TLensGlow();

  virtual void perform(u32, JDrama::TGraphics *) override;
  u32 _10[3];
  J3DFrameCtrl mGlowBtk;
  u32 _30;
  J3DFrameCtrl mGlowBrk;
  u32 _48[0x20 / 4];
  f32 mViewingSize;   // 0x0068
  f32 mPeekingSize;   // 0x006C
  f32 mSizeLerpSpeed; // 0x0070
  u32 _70[3];
  f32 mOffsetFromPosX; // 0x0080
  f32 mOffsetFromPosY; // 0x0084
  u32 _88[2];
  f32 mOffsetReturnSpeed; // 0x0090
  u32 _94;
};