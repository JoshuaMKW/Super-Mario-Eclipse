#pragma once

#include "types.h"
#include "FlagT.hxx"
#include "ViewObj.hxx"
#include "../JGeometry.hxx"
#include "../JSU/JSUMemoryStream.hxx"

namespace JDrama {

class TPlacement : public TViewObj {
public:
  TPlacement(const char *name)
      : TViewObj(name), mPosition(0.0f, 0.0f, 0.0f), mPlacementFlags(0) {}
  virtual ~TPlacement();

  virtual void load(JSUMemoryInputStream &);

  JGeometry::TVec3<f32> mPosition;
  TFlagT<u16> mPlacementFlags;
};

}