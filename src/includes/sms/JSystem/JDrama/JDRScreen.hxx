#pragma once

#include "types.h"

#include "JGeometry.hxx"
#include "JDRNameRef.hxx"
#include "JDRViewObj.hxx"
#include "JDRViewport.hxx"
#include "JDRViewConnector.hxx

#include "sms/camera/CamConnector.hxx"

namespace JDrama {

class TScreen : public TViewConnector {
public:
  TScreen(const TRect &, const char *);
  virtual ~TScreen();

  void assignCamera(TViewObj *);
  void assignViewObj(TViewObj *);

  TViewport *mViewport;         // 10
  TCamConnector *mCamConnecter; // 14
};

} // namespace JDrama