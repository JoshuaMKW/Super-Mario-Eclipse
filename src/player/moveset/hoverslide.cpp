#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

#if SME_HOVER_SLIDE

using namespace SME;

static void checkHoverSlideFOV(CPolarSubCamera *camera, int mode, int sub, bool unk_1) {
  camera->changeCamModeSub_(mode, sub, unk_1);
}
//SME_PATCH_BL(SME_PORT_REGION(0x80021af8, 0, 0, 0), checkHoverSlideFOV);

#endif