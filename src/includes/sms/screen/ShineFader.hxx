#pragma once

#include "SmplFader.hxx"

class TShineFader : public TSmplFader {
public:
  virtual ~TShineFader();
  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void update() override;

  void registFadeOut(u16, u16);

  u16 _38;
};