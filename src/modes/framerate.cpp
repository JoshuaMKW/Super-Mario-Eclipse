#include "OS.h"
#include "SME.hxx"
#include "types.h"


using namespace SME;

static f32 sLastFPS = 30.0f;

// extern -> boot.cpp
void updateFPSAsync(OSAlarm *alarm, OSContext *context) {
  const f32 curFPS = TGlobals::isVariableFrameRate() ? TGlobals::getFrameRate()
                                                     : TGlobals::getFrameRate();
  if (curFPS != sLastFPS) {
    JDrama::TDisplay *display = gpApplication.mDisplay;
    if (display) {
      const f32 factor = curFPS / 30.0f;
      *(f32 *)SME_PORT_REGION(0x804167B8, 0, 0, 0) = 0.5f * factor;
      *(f32 *)SME_PORT_REGION(0x80414904, 0, 0, 0) = 0.001f * factor;
      display->mRetraceCount = curFPS > 30.0f ? 1 : 2;

      sLastFPS = curFPS;
    }
  }
}

// 044167b8 3f800000
// 042fcb24 60000000
// 04414904 3ca3d70a
// c20066ec 00000002
// c2c28028 ec2105b2
// fec00890 00000000