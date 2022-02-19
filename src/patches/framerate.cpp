#include "types.h"
#include "SME.hxx"

static f32 setBoidSpeed(f32 thing) {
    return sqrt__Q29JGeometry8TUtil_f(thing) * (30.0f / SME::TGlobals::getFrameRate());
}
SME_PATCH_BL(SME_PORT_REGION(0x800066E4, 0, 0, 0), setBoidSpeed);

static void requestFadeTimeScaled(TSMSFader *fader, TSMSFader::WipeRequest *request) {
    #if 0
    const f32 scale = (SME::TGlobals::getFrameRate() / 30.0f);
    request->mWipeSpeed *= scale;
    request->mDelayTime *= scale;
    #endif
    fader->requestWipe(request);
}
SME_PATCH_BL(SME_PORT_REGION(0x8013FE84, 0, 0, 0), requestFadeTimeScaled);