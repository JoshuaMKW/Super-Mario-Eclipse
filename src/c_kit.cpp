#include "CARD.h"
#include "printf.h"
#include "sms/JSystem/J2D/J2DOrthoGraph.hxx"
#include "sms/JSystem/J2D/J2DTextBox.hxx"
#include "sms/talk/Talk2D2.hxx"
#include "string.h"


#include "SME.hxx"

using namespace SME;

extern J2DTextBox gDebugTextBox;

bool inXYZMode;

/*
static void xyzModifierMario()
{
    #ifndef SME_DEBUG
        if (!SME::Class::TCheatHandler::sDebugHandler.isActive())
            return;
    #endif

    if (!gpMarioAddress)
        return;

    if ((void *)gpMarioAddress->mController < (void *)0x80000000 || (u32
*)gpMarioAddress->mController >= (void *)0x81800000) return;

    if (gpMarioAddress->mController->mButtons.mFrameInput &
TMarioGamePad::Buttons::DPAD_RIGHT && !inXYZMode)
    {
        inXYZMode = true;
    }
    else if (gpMarioAddress->mController->mButtons.mFrameInput &
TMarioGamePad::Buttons::DPAD_RIGHT && inXYZMode)
    {
        gpMarioAddress->mState = TMario::State::IDLE;
        inXYZMode = false;
    }

    if (inXYZMode)
    {
        gpMarioAddress->mState = TMario::State::IDLE | TMario::State::CUTSCENE;
        f32 speedMultiplier = lerp<f32>(1, 2,
gpMarioAddress->mController->mButtons.mAnalogR); gpMarioAddress->mPosition.x +=
((gpMarioAddress->mController->mControlStick.mStickX * 83) * speedMultiplier);
        gpMarioAddress->mPosition.z -=
((gpMarioAddress->mController->mControlStick.mStickY * 83) * speedMultiplier);

        if (gpMarioAddress->mController->mButtons.mInput &
TMarioGamePad::Buttons::DPAD_DOWN)
        {
            gpMarioAddress->mPosition.y -= (83 * speedMultiplier);
        }
        else if (gpMarioAddress->mController->mButtons.mInput &
TMarioGamePad::Buttons::DPAD_UP)
        {
            gpMarioAddress->mPosition.y += (83 * speedMultiplier);
        }
    }
    return;
}
*/

// this is ran once
// extern -> SME.cpp
void Patch::CKit::onSetup(TMarDirector *director) {
  gDebugTextBox = J2DTextBox(gpSystemFont->mFont, "Debug Mode");

  // run replaced call
  director->setupObjects();
}

// this is ran every frame
// extern -> SME.cpp
s32 Patch::CKit::onUpdate(void *director) { //movie director
  // xyzModifierMario();
  // Patch::Cheat::drawCheatText(); //currently bugged

  // run replaced call
  u32 func;
  SME_FROM_GPR(12, func);
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
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC ==
      SME::Class::TSMEFile::MAGIC)
    return SME::Class::TSMEFile::sStageConfig.GlobalFlags.StageType.mIsExMap;
  else
    return (gpApplication.mCurrentScene.mAreaID >= TGameSequence::DOLPICEX0 &&
            gpApplication.mCurrentScene.mAreaID <= TGameSequence::COROEX6);
}

// 0x802A8B30
// extern -> SME.cpp
bool Util::SMS::isMultiplayerMap() {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC ==
      SME::Class::TSMEFile::MAGIC)
    return SME::Class::TSMEFile::sStageConfig.GlobalFlags.StageType
        .mIsMultiPlayerMap;
  else
    return (gpMarDirector->mAreaID == TGameSequence::TEST10 &&
            gpMarDirector->mEpisodeID == 0);
}

// 0x802A8AFC
// extern -> SME.cpp
bool Util::SMS::isDivingMap() {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC ==
      SME::Class::TSMEFile::MAGIC)
    return SME::Class::TSMEFile::sStageConfig.GlobalFlags.StageType
        .mIsDivingMap;
  else
    return (gpMarDirector->mAreaID == TGameSequence::MAREBOSS ||
            gpMarDirector->mAreaID == TGameSequence::MAREEX0 ||
            gpMarDirector->mAreaID == TGameSequence::MAREUNDERSEA);
}

// 0x802A8AE0
// extern -> SME.cpp
bool Util::SMS::isOptionMap() {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC ==
      SME::Class::TSMEFile::MAGIC)
    return SME::Class::TSMEFile::sStageConfig.GlobalFlags.StageType
        .mIsOptionMap;
  else
    return (gpMarDirector->mAreaID == 15);
}

// 0x8027C6A4
// extern -> SME.cpp
bool Patch::CKit::manageLightSize() {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsShineShadow)
    return (gpMarDirector->mAreaID == 1);

  s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
  s32 &PrevShineCount = SME::TGlobals::sGlobals.mLightData.mPrevShineCount;
  switch (SME::TGlobals::sGlobals.mLightData.mLightType) {
  case SME::Enum::LightContext::STATIC: {
    if (SME::Class::TSMEFile::sStageConfig.Light.mDarkLevel != 255)
      gpModelWaterManager->mDarkLevel =
          SME::Class::TSMEFile::sStageConfig.Light.mDarkLevel;
    else if (CurrentShineCount < SME_MAX_SHINES)
      gpModelWaterManager->mDarkLevel =
          SME::Util::Math::lerp<u8>(30, 190,
                                    static_cast<f32>(CurrentShineCount) /
                                        static_cast<f32>(SME_MAX_SHINES));
    else {
      if (gpModelWaterManager->mDarkLevel < 255)
        gpModelWaterManager->mDarkLevel += 1;
      else
        SME::TGlobals::sGlobals.mLightData.mLightType =
            SME::Enum::LightContext::DISABLED;
    }

    gShineShadowPos =
        SME::TGlobals::sGlobals.mLightData.mShineShadowCoordinates;

    f32 sigOfs = 300.0f;
    f32 sigStrength = CurrentShineCount >= PrevShineCount ? 0.04f : -0.04f;

    if (!SME::TGlobals::sGlobals.mLightData.mSizeMorphing &&
        CurrentShineCount == PrevShineCount)
      break;

    if (CurrentShineCount > PrevShineCount) {
      SME::TGlobals::sGlobals.mLightData.mPrevSize = gpModelWaterManager->mSize;
      SME::TGlobals::sGlobals.mLightData.mNextSize = gpModelWaterManager->mSize;

      for (u32 i = 0; i < (CurrentShineCount - PrevShineCount); ++i)
        SME::TGlobals::sGlobals.mLightData.mNextSize +=
            (10000.0f / SME_MAX_SHINES) + (PrevShineCount + i) * 2.0f;

      SME::TGlobals::sGlobals.mLightData.mSizeMorphing = true;
      SME::TGlobals::sGlobals.mLightData.mStepContext = 0.0f;
    } else if (CurrentShineCount < PrevShineCount) {
      SME::TGlobals::sGlobals.mLightData.mPrevSize = gpModelWaterManager->mSize;
      SME::TGlobals::sGlobals.mLightData.mNextSize = gpModelWaterManager->mSize;

      for (u32 i = 0; i < (PrevShineCount - CurrentShineCount); ++i)
        SME::TGlobals::sGlobals.mLightData.mNextSize -=
            (10000.0f / SME_MAX_SHINES) + (PrevShineCount - i) * 2.0f;

      SME::TGlobals::sGlobals.mLightData.mSizeMorphing = true;
      SME::TGlobals::sGlobals.mLightData.mStepContext = 0.0f;
    }

    f32 cur = SME::Util::Math::sigmoidCurve(
        SME::TGlobals::sGlobals.mLightData.mStepContext,
        SME::TGlobals::sGlobals.mLightData.mPrevSize,
        SME::TGlobals::sGlobals.mLightData.mNextSize, sigOfs, sigStrength);

    if (gpModelWaterManager->mSize > 70000.0f) {
      gpModelWaterManager->mSize = 70000.0f;
      SME::TGlobals::sGlobals.mLightData.mSizeMorphing = false;
    } else if (gpModelWaterManager->mSize < 0.0f) {
      gpModelWaterManager->mSize = 0.0f;
      SME::TGlobals::sGlobals.mLightData.mSizeMorphing = false;
    } else if (cur != SME::TGlobals::sGlobals.mLightData.mNextSize &&
               cur != SME::TGlobals::sGlobals.mLightData.mPrevSize) {
      gpModelWaterManager->mSize = cur;
      gpModelWaterManager->mSphereStep = cur / 2.0f;
      SME::TGlobals::sGlobals.mLightData.mStepContext += 1.0f;
    } else {
      gpModelWaterManager->mSize = cur;
      gpModelWaterManager->mSphereStep = cur / 2.0f;
      PrevShineCount = CurrentShineCount;
      SME::TGlobals::sGlobals.mLightData.mSizeMorphing = false;
    }
    break;
  }
  case SME::Enum::LightContext::FOLLOWPLAYER: {
    gpModelWaterManager->mDarkLevel =
        SME::Class::TSMEFile::sStageConfig.Light.mDarkLevel;
    gShineShadowPos.x =
        gpMarioPos->x +
        SME::TGlobals::sGlobals.mLightData.mShineShadowCoordinates.x;
    gShineShadowPos.y =
        gpMarioPos->y +
        SME::TGlobals::sGlobals.mLightData.mShineShadowCoordinates.y;
    gShineShadowPos.z =
        gpMarioPos->z +
        SME::TGlobals::sGlobals.mLightData.mShineShadowCoordinates.z;
    break;
  }
  default:
    break;
  }
  return SME::TGlobals::sGlobals.mLightData.mLightType !=
             SME::Enum::LightContext::DISABLED &&
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
    *(f32 *)0x80415F4C = 480.0f; // tounge
    *(f32 *)0x80415F68 = 16384.0f;
  } else {
    *(f32 *)0x80415F4C = 300.0f;
    *(f32 *)0x80415F68 = 10000.0f;
  }
}

// 0x8024D3A8
// extern -> SME.cpp
void Patch::CKit::realTimeCustomAttrsHandler(TMario *player) {
  maintainYoshi(player->mYoshi);
  setPositions__6TMarioFv(player);
}