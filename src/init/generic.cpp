#include "JDrama/JDRNameRef.hxx"
#include "JKR/JKRDecomp.hxx"
#include "JKR/JKRHeap.hxx"
#include "JKR/JKRMemArchive.hxx"
#include "printf.h"
#include "stdlib.h"

#include "SME.hxx"

using namespace SME;
using namespace Util;
using namespace Class;

// 0x80280180
static void initShineShadow() {
  TStageParams *config = TStageParams::sStageConfig;
  if (config->mLightType.get() == TLightContext::ActiveType::DISABLED)
    return;

  Class::TLightContext &LightContext = TGlobals::sLightData;

  s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
  if (CurrentShineCount < SME_MAX_SHINES) {
    LightContext.mLightType = config->mLightType.get();
    LightContext.mShineShadowBase = config->mLightSize.get();
    LightContext.mPrevShineCount = CurrentShineCount;
    {
      JGeometry::TVec3<f32> coordinates(config->mLightPosX.get(),
                                        config->mLightPosY.get(),
                                        config->mLightPosZ.get());
      LightContext.mShineShadowCoordinates = coordinates;
    }

    gpModelWaterManager->mColor = config->mLightColor.get();
    gpModelWaterManager->mDarkLevel = config->mLightDarkLevel.get();
    gpModelWaterManager->mLayerCount = config->mLightLayerCount.get();
    gpModelWaterManager->mSphereStep = config->mLightStep.get();
    gpModelWaterManager->mSize = config->mLightSize.get();
    gpModelWaterManager->LightType.mMaskObjects = true;
    gpModelWaterManager->LightType.mShowShadow = true;

    if (LightContext.mLightType == TLightContext::ActiveType::STATIC) {
      LightContext.mNextSize =
          LightContext.mShineShadowBase +
          powf(((1350.0f / SME_MAX_SHINES) * CurrentShineCount), 1.5f);

      if (config->mLightDarkLevel.get() == 255)
        gpModelWaterManager->mDarkLevel =
            Util::Math::lerp<u8>(TGlobals::getMinDarkness(), 190,
                                 static_cast<f32>(CurrentShineCount) /
                                     static_cast<f32>(SME_MAX_SHINES));
      else
        gpModelWaterManager->mDarkLevel = config->mLightDarkLevel.get();

      gpModelWaterManager->mSize = LightContext.mNextSize;
      gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
    }
  } else {
    LightContext.mLightType = TLightContext::ActiveType::DISABLED;
  }
}
SME_PATCH_B(SME_PORT_REGION(0x80280180, 0, 0, 0), initShineShadow);

/*This works by taking the target id and matching it to the
/ ID of the first entry to have the same home ID as the target.
/
/ This makes a half decent linking system for the collision
/ triangles for functionalities like linked warping!
*/

static void parseWarpLinks(TMapCollisionData *col, TWarpCollisionList *links,
                           u32 validID, u32 idGroupSize = 0) {
  u32 curDataIndex = 0;

  for (u32 i = 0; i < col->mFloorArraySize; ++i) {
    if (TCollisionLink::isValidWarpCol(&col->mColTable[i])) {

      TCollisionLink link(
          &col->mColTable[i], (u8)(col->mColTable[i].mValue4 >> 8),
          (u8)col->mColTable[i].mValue4,
          TCollisionLink::getSearchModeFrom(&col->mColTable[i]));

      links->addLink(link);
      if (curDataIndex > 0xFF)
        break;
    }
  }
}

// 0x802B8B20
static u32 initCollisionWarpLinks(const char *name) {
  TWarpCollisionList *warpDataArray = new TWarpCollisionList(2048);
  TWarpCollisionList *warpDataPreserveArray = new TWarpCollisionList(1);
  TGlobals::sWarpColArray = warpDataArray;
  TGlobals::sWarpColPreserveArray = warpDataPreserveArray;

  if (warpDataArray) {
    parseWarpLinks(gpMapCollisionData, warpDataArray, 16040, 4);
  }

  return JDrama::TNameRef::calcKeyCode(name);
}
SME_PATCH_BL(SME_PORT_REGION(0x802B8B20, 0, 0, 0), initCollisionWarpLinks);