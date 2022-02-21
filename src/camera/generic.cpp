#include "sms/camera/PolarSubCamera.hxx"

#include "SME.hxx"
#include "types.h"

using namespace SME;

static void updateSlideFOV(CPolarSubCamera *); // Forward decl

static bool updateContexts(CPolarSubCamera *cam) {
  updateSlideFOV(cam);
  return cam->isNormalDeadDemo();
}
SME_PATCH_BL(SME_PORT_REGION(0x80023598, 0, 0, 0), updateContexts);

/* CONTEXTS */

static f32 sLastFactor = 1.0f;
static void updateSlideFOV(CPolarSubCamera *cam) {
  f32 factor = Math::scaleLinearAtAnchor<f32>(
      gpMarioAddress->mForwardSpeed / 100.0f, 0.5f, 1.0f);

  factor = Math::lerp<f32>(sLastFactor, factor, 0.01f);

  if (factor > 1.0f &&
      gpMarioAddress->mState == static_cast<u32>(TMario::State::DIVESLIDE)) {
    sLastFactor = factor;
    reinterpret_cast<f32 *>(cam)[0x48 / 4] *= factor;
  } else {
    sLastFactor = Math::lerp<f32>(sLastFactor, 1.0f, 0.01f);
    reinterpret_cast<f32 *>(cam)[0x48 / 4] *= sLastFactor;
  }
}

/* HOOKS */

static void modifyCameraRangeToSize(f32 *params, f32 *saveParams) {
  params[0xA8 / 4] = saveParams[0x3B0 / 4];

  // Custom code here
  const Class::TPlayerData *playerParams =
      TGlobals::getPlayerData(gpMarioAddress);
  const f32 scale = playerParams->getParams()->mSizeMultiplier.get();

  if (!gpMarioAddress->mYoshi ||
      gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED || scale > 1.0f) {
    params[0x8 / 4] *= Util::Math::scaleLinearAtAnchor<f32>(scale, 0.5f, 1.0f);
    params[0xC / 4] *= Util::Math::scaleLinearAtAnchor<f32>(scale, 0.5f, 1.0f);
    params[0x24 / 4] *=
        Util::Math::scaleLinearAtAnchor<f32>(scale, 0.9375f, 1.0f);
  }
}
SME_PATCH_B(SME_PORT_REGION(0x80027548, 0, 0, 0), modifyCameraRangeToSize);