#pragma once

#include "JSU/JSUMemoryStream.hxx"
#include "MapObjRail.hxx"
#include "types.h"

class TNormalLift : public TRailMapObj {
public:
  TNormalLift(const char *);
  virtual ~TNormalLift();

  virtual void control() override;
  virtual void initMapObj() override;
  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void readRailFlag() override;
  virtual void resetPosition() override;
  virtual void setGroundCollision() override;

  u16 _150;
  u16 _152;
  f32 _154;
  u8 _158;
};