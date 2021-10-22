#pragma once

#include "types.h"
#include "JDRNameRef.hxx"
#include "JDRViewObj.hxx"
#include "JGeometry.hxx"

namespace JDrama {

class TViewConnector : public TViewObj {
public:
  TViewConnector(TViewObj *viewObj1, TViewObj *viewObj2, TFlagT<u16> flags,
                 const char *objName);
  virtual ~TViewConnector();

  virtual void perform(u32, TGraphics *);

  TViewObj *mViewObj1; // 10
  TViewObj *mViewObj2; // 14
  u16 mFlag;           // 18
};

}