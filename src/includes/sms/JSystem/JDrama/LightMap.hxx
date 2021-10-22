#pragma once

#include "types.h"
#include "Graphics.hxx"
#include "ViewObj.hxx"
#include "../JSU/JSUMemoryStream.hxx"

namespace JDrama {

class TLightMap : public TViewObj {
public:
  class TLightInfo {
  public:
    TLightInfo();

    u32 _0;
    u32 _4;
  };

  virtual ~TLightMap();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  s32 mLightArryCount;                   // 10
  TLightMap::TLightInfo *mLightInfoArry; // 14
};

}