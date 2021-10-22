#pragma once

#include "types.h"
#include "JDRViewObj.hxx"

#include "../JStage/JSGAmbientLight.hxx"

namespace JDrama {

class TAmbColor : public TViewObj, public JStage::TAmbientLight {
public:
  TAmbColor();
  virtual ~TAmbColor();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  u8 r; // 14
  u8 g; // 15
  u8 b; // 16
  u8 a; // 17
};

}