#include "CARD.h"
#include "printf.h"
#include "sms/JSystem/J2D/J2DOrthoGraph.hxx"
#include "sms/JSystem/J2D/J2DTextBox.hxx"
#include "sms/talk/Talk2D2.hxx"
#include "string.h"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

extern J2DTextBox gDebugTextBox;

// this is ran once
// extern -> SME.cpp
void Patch::CKit::onSetup(TMarDirector *director) {
  gDebugTextBox = J2DTextBox(gpSystemFont->mFont, "Debug Mode");

  // run replaced call
  director->setupObjects();
}

// this is ran every frame
// extern -> SME.cpp
s32 Patch::CKit::onUpdate(void *director) { // movie director
  // xyzModifierMario();
  // Patch::Cheat::drawCheatText(); //currently bugged

  // run replaced call
  u32 func;
  SME_FROM_GPR(12, func);

  TMarioGamePad *controller = gpApplication.mGamePad1;
  if ((controller->mButtons.mInput & 0x260) == 0x260) { // L + R + B + D-PAD UP
    SME::Util::Mario::switchCharacter(gpMarioAddress,
                                      SME::Util::Mario::getPlayerIDFromInput(
                                          controller->mButtons.mInput & 0xF),
                                      true);
  }
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
            Util::Math::lerp<u8>(30, 190,
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
              static_cast<f32>(LightContext.mNextDarkness), sigOfs, sigStrength));

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

    if (gpMario->mState != TMario::State::IDLE)
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
void Patch::CKit::formatTalkMessage(Talk2D2 *talker, char *msgfield,
                                    u32 *entrydata) {
  String fmtMessage(1024);

  const char *basemsg = msgfield + *entrydata + talker->curMsgIndex;
  const char *newmsg = fmtMessage.data() - (*entrydata + talker->curMsgIndex);

  fmtMessage.assign(basemsg);
  SME::Util::formatBMG(fmtMessage);

  setupTextBox__8TTalk2D2FPCvP12JMSMesgEntry(talker, newmsg, entrydata);
}

static void maintainYoshi(TYoshi *yoshi) {
  if (yoshi->isGreenYoshi()) {
    *(f32 *)SME_PORT_REGION(0x80415F4C, 0x8040d4a4, 0, 0) = 480.0f; // tounge
    *(f32 *)SME_PORT_REGION(0x80415F68, 0x8040d4a8, 0, 0) = 16384.0f;
  } else {
    *(f32 *)SME_PORT_REGION(0x80415F4C, 0x8040d4a4, 0, 0) = 300.0f;
    *(f32 *)SME_PORT_REGION(0x80415F68, 0x8040d4a8, 0, 0) = 10000.0f;
  }
}

// 0x8024D3A8
// extern -> SME.cpp
void Patch::CKit::realTimeCustomAttrsHandler(TMario *player) {
  if(player->mYoshi){
        maintainYoshi(player->mYoshi);
  }

  Class::TPlayerData *playerParams = TGlobals::getPlayerParams(player);
  Class::TStageParams *stageParams = Class::TStageParams::sStageConfig;

  const Class::TPlayerParams *params = playerParams->getParams();
  //const Class::TPlayerData::ParamHistory &defaults = playerParams->mDefaultAttrs;
  const f32 sizeMultiplier = params->mSizeMultiplier.get() * stageParams->mPlayerSizeMultiplier.get();
  const f32 speedMultiplier = params->mSpeedMultiplier.get();
  //const f32 diminishedSizeMultiplier = SME::Util::Math::scaleLinearAtAnchor(sizeMultiplier, 0.5f, 1.0f);

  playerParams->scalePlayerAttrs(sizeMultiplier);

  setPositions__6TMarioFv(player);
}