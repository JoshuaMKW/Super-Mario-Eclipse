#pragma once

#include "types.h"
#include "JDRFlagT.hxx"
#include "JDRViewObj.hxx"
#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"

namespace JDrama {

class TPlacement : public TViewObj {
public:
  TPlacement(const char *name)
      : TViewObj(name), mPosition(0.0f, 0.0f, 0.0f), mPlacementFlags(0) {}
  virtual ~TPlacement();

  virtual void load(JSUMemoryInputStream &);

  TVec3f mPosition;
  TFlagT<u16> mPlacementFlags;
};

}