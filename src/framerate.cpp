#include "types.h"
#include "SME.hxx"

#if 0

static void setFrameRateThings() {
  const f32 frameRate = SME::TGlobals::isVariableFrameRate()
                            ? SME::TGlobals::getFrameRate()
                            : SME::TGlobals::getFrameRate();
  *(f32 *)SME_PORT_REGION(0x804167B8, 0, 0, 0) = 0.5f * (frameRate / 30.0f);
  *(f32 *)SME_PORT_REGION(0x80414904, 0, 0, 0) = 0.01f * (frameRate / 30.0f);
}

static f32 setBoidSpeed(f32 thing) {
    return sqrt__Q29JGeometry8TUtil_f(thing) * (30.0f / SME::TGlobals::getFrameRate());
}
SME_PATCH_BL(SME_PORT_REGION(0x800066E4, 0, 0, 0), setBoidSpeed);

SME_WRITE_32(SME_PORT_REGION(0x802FCB24, 0, 0, 0), 0x60000000);

#endif