#pragma once

#include "types.h"
#include "JDRFlagT.hxx"
#include "JDRGraphics.hxx"
#include "JDRNameRef.hxx"

namespace JDrama {

class TViewObj : public TNameRef {
public:
  TViewObj(const char *);
  virtual ~TViewObj();

  virtual void perform(u32, TGraphics *) = 0;

  void testPerform(u32, TGraphics *);

  TFlagT<u16> mViewFlags; // 0x000C
};

}