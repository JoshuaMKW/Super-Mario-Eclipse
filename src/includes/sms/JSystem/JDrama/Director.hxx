#pragma once

#include "types.h"
#include "NameRef.hxx"
#include "../JStage.hxx"
#include "../JUT/JUTGamePad.hxx"
#include "sms/game/PerformList.hxx"

namespace JDrama {

class TDirector : public TNameRef, public JStage::TSystem {
public:
  virtual ~TDirector();

  virtual TDirector *searchF(u16, const char *);
  virtual JStage::TObject *
  JSGFindObject(const char *, JStage::TEObject) const; // JStage::TEObject
  virtual s32 direct();

  u32 *mViewObjPtrList; // TViewObjPtrList *
  u32 _14;
  JUTGamePad **mGamePads;
  TPerformList *mPerformListGX;
  TPerformList *mPerformListSilhouette;
};

}