#include "SME.hxx"
#include "sms/camera/LensFlare.hxx"
#include "sms/camera/LensGlow.hxx"
#include "sms/camera/SunModel.hxx"
#include "types.h"

using namespace SME;

static JGeometry::TVec3<f32> sSunBasePos(0.0f, 0.0f, 0.0f);
static u8 sSunBaseBlindStrength = 0;

static s16 captureSunData() {
  TSunModel *sun;
  SME_FROM_GPR(30, sun);

  JGeometry::TVec3<f32> spos;

  JSGGetTranslation__Q26JDrama6TActorCFP3Vec(sun,
                                             reinterpret_cast<Vec *>(&spos));
  sSunBasePos.set(spos);
  sSunBaseBlindStrength = sun->mBlindingStrength;

  return SMSGetAnmFrameRate__Fv();
}
SME_PATCH_BL(SME_PORT_REGION(0x8002F440, 0, 0, 0), captureSunData);

static bool sunFollowCameraAndScaleLightness(TCameraMarioData *cam) {
  TSunModel *sun;
  SME_FROM_GPR(29, sun);

  JGeometry::TVec3<f32> spos(sSunBasePos);
  JGeometry::TVec3<f32> cpos;

  JSGGetViewPosition__Q26JDrama13TLookAtCameraCFP3Vec(
      gpCamera, reinterpret_cast<Vec *>(&cpos));

  spos.add(cpos);
  JSGSetTranslation__Q26JDrama6TActorFRC3Vec(sun,
                                             reinterpret_cast<Vec &>(spos));

  sun->mBlindingStrength =
      Math::lerp<u8>(20, sSunBaseBlindStrength,
                     TGlobals::sLightData.mLightType !=
                             Class::TLightContext::ActiveType::DISABLED
                         ? static_cast<f32>(gpModelWaterManager->mDarkLevel) / 255.0f
                         : 1.0f);

  return cam->isMarioIndoor();
}
SME_PATCH_BL(SME_PORT_REGION(0x8002EB34, 0, 0, 0),
             sunFollowCameraAndScaleLightness);

static f32 scaleFlareToLightness(f32 a, f32 b, f32 c) {
  return CLBEaseOutInbetween_f(
      a,
      TGlobals::sLightData.mLightType !=
              Class::TLightContext::ActiveType::DISABLED
          ? static_cast<f32>(gpModelWaterManager->mDarkLevel)
          : 255.0f,
      c);
}
SME_PATCH_BL(SME_PORT_REGION(0x8002D358, 0, 0, 0), scaleFlareToLightness);

static bool scaleGlowToLightness(f32 a, f32 b, f32 c) {
  TLensGlow *glow;
  SME_FROM_GPR(28, glow);

  const f32 factor =
      TGlobals::sLightData.mLightType !=
              Class::TLightContext::ActiveType::DISABLED
          ? static_cast<f32>(gpModelWaterManager->mDarkLevel) / 255.0f
          : 1.0f;

  return CLBLinearInbetween_f(a * factor, b * factor, c);
}
SME_PATCH_BL(SME_PORT_REGION(0x8002DB60, 0, 0, 0), scaleGlowToLightness);