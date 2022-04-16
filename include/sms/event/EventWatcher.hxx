#pragma once

#include "types.h"

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "sms/spc/SpcTypedBinary.hxx"

class TEventWatcher : public JDrama::TViewObj {
public:
  TEventWatcher(const char *, const char *);
  TEventWatcher(const char *);
  virtual ~TEventWatcher();

  virtual void perform(u32, JDrama::TGraphics *);
  virtual void launchScript(const char *);

  TSpcTypedBinary<TEventWatcher> *_10;
  TSpcTypedBinary<TEventWatcher> *_14;
};

char *sShineViewObjName;