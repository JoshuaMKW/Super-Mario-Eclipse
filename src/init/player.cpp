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

static void initFludd(TMario *player, TPlayerData *params) {
  SME_ASSERT(params, "Can't init fludd with non existant params!");
  TStageParams *config = TStageParams::sStageConfig;

  if (!params->isMario())
    return;

  if (!params->canUseNozzle(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mCurrentNozzle))) {
    for (u8 i = 0; i < 7; ++i) {
      if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
        player->mAttributes.mHasFludd = params->getCanUseFludd();
        player->mFludd->mCurrentNozzle = i;
        player->mFludd->mCurrentWater =
            player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
                ->mMaxWater;
        break;
      } else if (i == 6) {
        player->mAttributes.mHasFludd = false;
        params->setCanUseFludd(false);
      }
    }
  }

  if (!params->canUseNozzle(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mSecondNozzle))) {
    for (u8 i = 0; i < 7; ++i) {
      if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
        player->mAttributes.mHasFludd = params->getCanUseFludd();
        player->mFludd->mSecondNozzle = i;
        break;
      }
      player->mFludd->mSecondNozzle = player->mFludd->mCurrentNozzle;
    }
  }

  if (config->mFluddShouldColorWater.get())
    gModelWaterManagerWaterColor = config->mFluddWaterColor.get();

  player->mFludd->mCurrentNozzle = config->mFluddPrimary.get();
  player->mFludd->mSecondNozzle = config->mFluddSecondary.get();

  player->mFludd->mCurrentWater =
      player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
          ->mMaxWater;
}

static void initFluddInLoadAfter(TWaterGun *fludd) {
  fludd->mNozzleList[4]->mDamageLoss = 250;
}
SME_PATCH_B(SME_PORT_REGION(0x8026A3B8, 0x80262144, 0, 0), initFluddInLoadAfter);

static void initMario(TMario *player, bool isMario) {
  TStageParams *config = TStageParams::sStageConfig;

  TPlayerData *params = new TPlayerData(player, nullptr, isMario);
  TGlobals::registerPlayerParams(params);

  params->scalePlayerAttrs(config->mPlayerSizeMultiplier.get());

  bool isGlasses = false;

  if (config->isCustomConfig()) {
    params->setPlayerID(isMario ? TGlobals::sCharacterIDList[0]
                                : Enum::Player::SHADOW_MARIO);
    player->mHealth = config->mPlayerHealth.get();
    player->mDeParams.mHPMax.set(config->mPlayerMaxHealth.get());
    player->mJumpParams.mGravity.set(config->mGravityMultiplier.get());

    if (isMario) {
      player->mAttributes.mGainHelmet = config->mPlayerHasHelmet.get();
      player->mAttributes.mHasFludd = config->mPlayerHasFludd.get();
      player->mAttributes.mIsShineShirt = config->mPlayerHasShirt.get();
    }

    isGlasses = config->mPlayerHasGlasses.get();
  }

  if (isMario) {
    player->mAttributes.mGainHelmet =
        params->getParams()->mPlayerHasHelmet.get();
    player->mAttributes.mHasFludd &= params->getParams()->mCanUseFludd.get();
    player->mAttributes.mIsShineShirt =
        params->getParams()->mPlayerHasShirt.get();
    isGlasses = params->getParams()->mPlayerHasGlasses.get();

    initFludd(player, params);
  }

  if (isGlasses && player->mCap)
    reinterpret_cast<u16 *>(player->mCap)[2] |= 0b100;
}

// 0x80276C94
static TMario *fromMarioInit(TMario *player) {
  SME_DEBUG_LOG("Initializing player values...\n");
  player->initValues();
  SME_DEBUG_LOG("Initializing Mario...\n");
  initMario(player, true);
  return player;
}
SME_WRITE_32(SME_PORT_REGION(0x80276C90, 0x8026EA1C, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x80276C94, 0x8026EA20, 0, 0), fromMarioInit);

// 0x800397DC
static bool fromShadowMarioInit() {
  TMario *player;
  asm volatile("lwz %0, 0x150 (31)" : "=r"(player));
  initMario(player, false);
  return SMS_isMultiPlayerMap__Fv();
}
SME_PATCH_BL(SME_PORT_REGION(0x800397DC, 0x80039894, 0, 0), fromShadowMarioInit);

// 0x80271580
static void initYoshi(MAnmSound *anmSound, void *r4, u32 r5, f32 f1) {
  initAnmSound__9MAnmSoundFPvUlf(anmSound, r4, r5, f1);

  TYoshi *yoshi;
  SME_FROM_GPR(31, yoshi);

  TStageParams *config = TStageParams::sStageConfig;
  if (!config->isCustomConfig())
    return;

  gYoshiJuiceColor[0] = config->mYoshiColorOrange.get();
  gYoshiJuiceColor[1] = config->mYoshiColorPurple.get();
  gYoshiJuiceColor[2] = config->mYoshiColorPink.get();

  gYoshiBodyColor[0] = config->mYoshiColorGreen.get();
  gYoshiBodyColor[1] = config->mYoshiColorOrange.get();
  gYoshiBodyColor[2] = config->mYoshiColorPurple.get();
  gYoshiBodyColor[3] = config->mYoshiColorPink.get();

  yoshi->mMaxJuice = config->mYoshiMaxJuice.get();
  yoshi->mMaxVSpdStartFlutter = config->mYoshiMaxVSpdStartFlutter.get();
  yoshi->mFlutterAcceleration = config->mYoshiFlutterAcceleration.get();
  yoshi->mMaxFlutterTimer = config->mYoshiMaxFlutterTimer.get();
}
SME_PATCH_BL(SME_PORT_REGION(0x80271580, 0x8026930C, 0, 0), initYoshi);