#pragma once

#include "sms/JSystem/JDrama.hxx"

class TStrategy : public JDrama::TViewObj {
public:
  TStrategy(const char *name);
  virtual ~TStrategy();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void loadAfter() override;
  virtual TNameRef *searchF(u16, const char *) override;

  u32 _10;
  u32 _14;
  u32 _18;
  u32 _1C;
  u32 _20;
  u32 _24;
  u32 _28;
  u32 _2C;
  u32 _30;
  u32 _34;
  u32 _38;
  u32 _3C;
  u32 _40;
  u32 _44;
  u32 _48;
  u32 _4C;
  u16 mHitObjFlags;
};