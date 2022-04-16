#pragma once

#include "sms/actor/HitActor.hxx"

class TEffectObjBase : public THitActor {
public:
  TEffectObjBase(const char *);
  virtual ~TEffectObjBase();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void init();
  virtual void reset();
  virtual void behaveToWater();
  virtual void moveObject();

  u32 _68;
  f32 _6C;
  f32 _70;
};