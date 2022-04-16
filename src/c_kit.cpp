#include "CARD.h"
#include "J2D/J2DOrthoGraph.hxx"
#include "J2D/J2DTextBox.hxx"
#include "J3D/J3DVertex.hxx"
#include "printf.h"
#include "sms/talk/Talk2D2.hxx"
#include "string.h"

#include "Globals.hxx"
#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

extern J2DTextBox gDebugTextBox;

// this is ran once
// extern -> SME.cpp
void Patch::CKit::onSetup(TMarDirector *director) {
  // gDebugTextBox = J2DTextBox(gpSystemFont->mFont, "Debug Mode");

  // run replaced call
  SME_DEBUG_LOG("Initializing Objects...\n");
  director->setupObjects();
  SME_DEBUG_LOG("Initializing Objects... DONE!\n");
}

extern void demoHandler(TMario *player);

// this is ran every frame
// extern -> SME.cpp
s32 Patch::CKit::onUpdate(void *director) { // movie director
  u32 func;
  SME_FROM_GPR(12, func);

  if (!gpMarioAddress)
    return ((s32(*)(void *))func)(director);

  if ((u32)gpMarioAddress->mController < 0x80000000 ||
      (u32)gpMarioAddress->mController >= (0x81800000 - sizeof(TMarioGamePad)))
    return ((s32(*)(void *))func)(director);

  if (!gpMarDirector) {
    return ((s32(*)(void *))func)(director);
  }

  if (gpMarDirector->mCurState == 0xA)
    return ((s32(*)(void *))func)(director);

  Debug::xyzModifierMario(gpMarioAddress);
  Debug::updateDebugCollision(gpMarioAddress);
  demoHandler(gpMarioAddress);

  TMarioGamePad *controller = gpApplication.mGamePad1;
  if ((controller->mButtons.mInput & 0x260) == 0x260) { // L + R + B + D-PAD UP
    // SME::Util::Mario::switchCharacter(gpMarioAddress,
    //                                  SME::Util::Mario::getPlayerIDFromInput(
    //                                      controller->mButtons.mInput & 0xF),
    //                                  true);
  }
  // run replaced call
  return ((s32(*)(void *))func)(director);
}

// this is ran when drawing is needed
// extern -> SME.cpp
void Patch::CKit::onDraw2D(J2DOrthoGraph *graph) {
  // run replaced call
  graph->setup2D();
}

// 0x802A8B58
// extern -> SME.cpp
bool Util::SMS::isExMap() {
  if (TStageParams::sStageConfig->isCustomConfig())
    return TStageParams::sStageConfig->mIsExStage.get();
  else
    return (gpApplication.mCurrentScene.mAreaID >= TGameSequence::DOLPICEX0 &&
            gpApplication.mCurrentScene.mAreaID <= TGameSequence::COROEX6);
}

// 0x802A8B30
// extern -> SME.cpp
bool Util::SMS::isMultiplayerMap() {
  if (TStageParams::sStageConfig->isCustomConfig())
    return TStageParams::sStageConfig->mIsMultiplayerStage.get();
  else
    return (gpMarDirector->mAreaID == TGameSequence::TEST10 &&
            gpMarDirector->mEpisodeID == 0);
}

// 0x802A8AFC
// extern -> SME.cpp
bool Util::SMS::isDivingMap() {
  if (TStageParams::sStageConfig->isCustomConfig())
    return TStageParams::sStageConfig->mIsDivingStage.get();
  else
    return (gpMarDirector->mAreaID == TGameSequence::MAREBOSS ||
            gpMarDirector->mAreaID == TGameSequence::MAREEX0 ||
            gpMarDirector->mAreaID == TGameSequence::MAREUNDERSEA);
}

// 0x802A8AE0
// extern -> SME.cpp
bool Util::SMS::isOptionMap() {
  if (TStageParams::sStageConfig->isCustomConfig())
    return TStageParams::sStageConfig->mIsOptionStage.get();
  else
    return (gpMarDirector->mAreaID == 15);
}

// 0x8027C6A4
// extern -> SME.cpp
bool Patch::CKit::manageLightSize() {
  if (!TStageParams::sStageConfig->isCustomConfig())
    return (gpMarDirector->mAreaID == 1);

  Class::TLightContext &LightContext = SME::TGlobals::sLightData;

  s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
  s32 &PrevShineCount = LightContext.mPrevShineCount;
  switch (LightContext.mLightType) {
  case TLightContext::ActiveType::STATIC: {
    if (TStageParams::sStageConfig->mLightDarkLevel.get() != 255)
      gpModelWaterManager->mDarkLevel =
          TStageParams::sStageConfig->mLightDarkLevel.get();
    else if (CurrentShineCount >= SME_MAX_SHINES) {
      if (gpModelWaterManager->mDarkLevel < 255)
        gpModelWaterManager->mDarkLevel += 1;
      else
        LightContext.mLightType = TLightContext::ActiveType::DISABLED;
    }

    gShineShadowPos = LightContext.mShineShadowCoordinates;

    const f32 sigOfs = 300.0f;
    const f32 sigStrength = 0.04f;

    if (!LightContext.mSizeMorphing) {
      if (CurrentShineCount != PrevShineCount) {
        LightContext.mPrevSize = gpModelWaterManager->mSize;
        LightContext.mNextSize =
            LightContext.mShineShadowBase +
            powf(((1350.0f / SME_MAX_SHINES) * CurrentShineCount), 1.5f);
        LightContext.mPrevDarkness = gpModelWaterManager->mDarkLevel;
        LightContext.mNextDarkness =
            Util::Math::lerp<u8>(TGlobals::getMinDarkness(), 190,
                                 static_cast<f32>(CurrentShineCount) /
                                     static_cast<f32>(SME_MAX_SHINES));

        LightContext.mSizeMorphing = true;
        LightContext.mStepContext = 0.0f;
      } else {
        break;
      }
    }

    const f32 cur = SME::Util::Math::sigmoidCurve(
        LightContext.mStepContext, LightContext.mPrevSize,
        LightContext.mNextSize, sigOfs, sigStrength);

    if (TStageParams::sStageConfig->mLightDarkLevel.get() == 255)
      gpModelWaterManager->mDarkLevel =
          static_cast<u8>(SME::Util::Math::sigmoidCurve(
              LightContext.mStepContext,
              static_cast<f32>(LightContext.mPrevDarkness),
              static_cast<f32>(LightContext.mNextDarkness), sigOfs,
              sigStrength));

    constexpr f32 deadZone = 1.0f;
    constexpr f32 minSize = 0.0f;
    constexpr f32 maxSize = 80000.0f;

    f32 &NextSize = LightContext.mNextSize;
    f32 &PrevSize = LightContext.mPrevSize;

    const bool isFinished = NextSize >= PrevSize ? cur >= (NextSize - deadZone)
                                                 : cur <= (NextSize + deadZone);
    if (gpModelWaterManager->mSize > maxSize) {
      gpModelWaterManager->mSize = maxSize;
      LightContext.mSizeMorphing = false;
    } else if (gpModelWaterManager->mSize < minSize) {
      gpModelWaterManager->mSize = minSize;
      LightContext.mSizeMorphing = false;
    } else if (!isFinished) {
      gpModelWaterManager->mSize = cur;
      gpModelWaterManager->mSphereStep = cur / 2.0f;
      LightContext.mStepContext += 1.0f;
    } else {
      gpModelWaterManager->mSize = cur;
      gpModelWaterManager->mSphereStep = cur / 2.0f;
      PrevShineCount = CurrentShineCount;
      LightContext.mSizeMorphing = false;
    }
    break;
  }
  case TLightContext::ActiveType::FOLLOWPLAYER: {
    gpModelWaterManager->mDarkLevel =
        TStageParams::sStageConfig->mLightDarkLevel.get();
    gShineShadowPos.x = gpMarioPos->x + LightContext.mShineShadowCoordinates.x;
    gShineShadowPos.y = gpMarioPos->y + LightContext.mShineShadowCoordinates.y;
    gShineShadowPos.z = gpMarioPos->z + LightContext.mShineShadowCoordinates.z;
    break;
  }
  default:
    break;
  }
  return LightContext.mLightType != TLightContext::ActiveType::DISABLED &&
         gpMarDirector->mAreaID != TGameSequence::OPTION;
}

// 0x802571F0
/*
f32 velocityCoordinatePatches(f32 floorCoordinateY)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mState != TMario::STATE_IDLE)
    { //Y storage
        gpMario->mSpeed.y = 0;
    }

    if (floorCoordinateY < gpMario->mPosition.y - 20)
    { //Downwarping
        floorCoordinateY = gpMario->mPosition.y;
    }
    asm("lfs 0, -0x0EC8 (2)");
    return floorCoordinateY;
}
*/

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
// 0x80004A6C
/*
f32 downWarpPatch(TMario *gpMario, f32 yVelocity)
{
    if (yVelocity < -100)
    {
        return gpMario->mSpeed.y;
    }
    else
    {
        return yVelocity;
    }
}
*/

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
/*
f32 upWarpPatch(TMario *gpMario, f32 yVelocity)
{
    if (yVelocity > 1000000)
    {
        return gpMario->mSpeed.y;
    }
    else
    {
        return yVelocity;
    }
}
*/

// MESSAGE MODIFICATIONS //

// 0x80153DE8, 0x80153E1C
// extern -> SME.cpp
static void extendedTagParam() {}
// SME_PATCH_BL(SME_PORT_REGION(0x80150c40, 0, 0, 0), extendedTagParam);

static void maintainYoshi(TYoshi *yoshi) {
  if (Util::Yoshi::isGreenYoshi(yoshi)) {
    *(f32 *)SME_PORT_REGION(0x80415F4C, 0x8040D4A4, 0, 0) = 480.0f; // tounge
    *(f32 *)SME_PORT_REGION(0x80415F68, 0x8040D4A8, 0, 0) = 16384.0f;
  } else {
    *(f32 *)SME_PORT_REGION(0x80415F4C, 0x8040D4A4, 0, 0) = 300.0f;
    *(f32 *)SME_PORT_REGION(0x80415F68, 0x8040D4A8, 0, 0) = 10000.0f;
  }
}

extern void blazePlayer(TMario *player);

// 0x8024D3A8
// 0x8003F8F0
// extern -> SME.cpp
void Patch::CKit::realTimeCustomAttrsHandler(TMario *player) {
  if (player->mYoshi)
    maintainYoshi(player->mYoshi);

  Class::TPlayerData *playerParams = TGlobals::getPlayerData(player);
  Class::TStageParams *stageParams = Class::TStageParams::sStageConfig;

  const Class::TPlayerParams *params = playerParams->getParams();
  const f32 sizeMultiplier =
      params->mSizeMultiplier.get() * stageParams->mPlayerSizeMultiplier.get();
  const f32 speedMultiplier = params->mSpeedMultiplier.get();

  playerParams->scalePlayerAttrs(sizeMultiplier);

#define SCALE_PARAM(param, scale) param.set(param.get() * scale)

  switch (playerParams->getPlayerID()) {
  case Enum::Player::MARIO:
  case Enum::Player::LUIGI:
  case Enum::Player::IL_PIANTISSIMO:
  case Enum::Player::SHADOW_MARIO:
    break;
  case Enum::Player::DRY_BONES:
    SCALE_PARAM(player->mRunParams.mMotBlendRunSp, 100.0f);
    SCALE_PARAM(player->mSwimParams.mWaitBouyancy, -1.0f);
    SCALE_PARAM(player->mSwimParams.mMoveBouyancy, 0.0f);
    SCALE_PARAM(player->mSwimParams.mPaddleDown, 2.0f);
    SCALE_PARAM(player->mSwimParams.mPaddleSpeedUp, 0.5f);
    break;
  default:
    break;
  }

  if (playerParams->mIsOnFire) {
    SCALE_PARAM(player->mDeParams.mRunningMax, 1.4f);
    SCALE_PARAM(player->mDeParams.mDashMax, 1.4f);
    SCALE_PARAM(player->mDeParams.mClashSpeed, 1.4f);
    SCALE_PARAM(player->mJumpParams.mJumpSpeedAccelControl, 1.4f);
    SCALE_PARAM(player->mJumpParams.mFenceSpeed, 1.4f);
    SCALE_PARAM(player->mRunParams.mMaxSpeed, 1.4f);
    SCALE_PARAM(player->mRunParams.mAddBase, 1.4f);
    SCALE_PARAM(player->mRunParams.mDecBrake, 1.05f);
    SCALE_PARAM(player->mRunParams.mRunAnmSpeedBase, 1.4f);
    SCALE_PARAM(player->mHangFenceParams.mMoveSp, 1.4f);
    SCALE_PARAM(player->mHangFenceParams.mDescentSp, 1.4f);
    SCALE_PARAM(player->mDirtyParams.mSlipRunSp, 1.4f);
    SCALE_PARAM(player->mDirtyParams.mSlipCatchSp, 1.4f);

    if (player->mAttributes.mIsShallowWater || player->mAttributes.mIsWater)
      Util::Mario::extinguishPlayer(player, false);
    else
      blazePlayer(player);
  }

#undef SCALE_PARAM

  setPositions__6TMarioFv(player);
}