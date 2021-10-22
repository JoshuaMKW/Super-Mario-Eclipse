#pragma once

#include "J3D/J3DAnimation.hxx"
#include "JDrama/JDRGraphics.hxx"

class TLensFlare : public JDrama::TViewObj {
public:
  TLensFlare(const char *name);
  virtual ~TLensFlare();

  virtual void perform(u32, JDrama::TGraphics *) override;
  u32 _10[0x3C / 4];
};