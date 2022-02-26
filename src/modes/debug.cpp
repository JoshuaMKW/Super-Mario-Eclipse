#include "MTX.h"
#include "types.h"

#include "obj/WaterBalloon.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Util;

/* CHEAT HANDLER */

static u16 gDebugModeCheatCode[] = {
    TMarioGamePad::DPAD_UP,   TMarioGamePad::DPAD_UP,
    TMarioGamePad::DPAD_DOWN, TMarioGamePad::DPAD_DOWN,
    TMarioGamePad::DPAD_LEFT, TMarioGamePad::DPAD_RIGHT,
    TMarioGamePad::DPAD_LEFT, TMarioGamePad::DPAD_RIGHT,
    TMarioGamePad::B,         TMarioGamePad::A,
    TMarioGamePad::START};

J2DTextBox gDebugTextBox;
Class::TCheatHandler gDebugHandler;

static void debugModeNotify(Class::TCheatHandler *) {
  if (gpMSound->gateCheck(MSound::SE_SHINE_TOUCH))
    startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(
        gpMSound, MSound::SE_SHINE_TOUCH, 0, 0, 0, 4);

  TGlobals::setDebugMode(true);
  SME_DEBUG_LOG("Debug mode activated!\n");

  // if (!TGlobals::isDebugMode())
  //   gDebugTextBox.mIsVisible = true;
}

// extern runtime_mods.cpp
static void drawCheatText() {
//   if (!gDebugTextBox.getStringPtr())
//     return;

// #if !SME_DEBUG
//     if (*gDebugTextBox.getStringPtr() != '\0' && gDebugHandler.isActive())
// #else
//     if (*gDebugTextBox.getStringPtr() != '\0')
// #endif
//     {
//       gDebugTextBox.draw(250, 24);
//     }
}

static void *handleDebugCheat(void *GCLogoDir) {
  if (!gDebugHandler.isInitialized()) {
    gDebugHandler.setGamePad(gpApplication.mGamePad1);
    gDebugHandler.setInputList(gDebugModeCheatCode);
    gDebugHandler.setSuccessCallBack(&debugModeNotify);
  }
  gDebugHandler.advanceInput();
  return GCLogoDir;
}
SME_PATCH_B(SME_PORT_REGION(0x80295B6C, 0, 0, 0), handleDebugCheat);

/* DEBUG MODS */

static void isLevelSelectAvailable() {
  u32 context;
  SME_FROM_GPR(30, context);

  if (context == 9 || context == 4)
    context = TGlobals::isDebugMode() ? 9 : 4;
  gpApplication.mContext = context;
}
SME_PATCH_BL(SME_PORT_REGION(0x802A6794, 0, 0, 0), isLevelSelectAvailable);


bool gInXYZMode = false;

// extern c_kit.cpp
// 0x8024D194
void Patch::Debug::xyzModifierMario(TMario *player) {
  constexpr f32 baseSpeed = 83.0f;
  constexpr u32 buttons = TMarioGamePad::EButtons::DPAD_UP;

  if (!TGlobals::isDebugMode())
    return;

  const JUTGamePad::CStick &mainStick = player->mController->mControlStick;
  const f32 speedMultiplier =
      Math::lerp<f32>(1, 2, player->mController->mButtons.mAnalogR);

  if (player->mController->mButtons.mFrameInput & buttons && !gInXYZMode) {
    gInXYZMode = true;
  } else if (player->mController->mButtons.mFrameInput & buttons &&
             gInXYZMode) {
    gInXYZMode = false;
  }

  if (gInXYZMode) {
    const f32 cameraRotY = (f32)(gpCamera->mHorizontalAngle) / 182.0f;

    Vec playerPos;
    player->JSGGetTranslation(&playerPos);

    playerPos.x +=
        ((-sinf(Math::angleToRadians(cameraRotY)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickY;
    playerPos.z +=
        ((-cosf(Math::angleToRadians(cameraRotY)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickY;
    playerPos.x -=
        ((-sinf(Math::angleToRadians(cameraRotY + 90.0f)) *
          baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;
    playerPos.z -=
        ((-cosf(Math::angleToRadians(cameraRotY + 90.0f)) *
          baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;

    if (player->mController->mButtons.mInput & TMarioGamePad::EButtons::B) {
      playerPos.y -= (baseSpeed * speedMultiplier);
    } else if (player->mController->mButtons.mInput &
               TMarioGamePad::EButtons::A) {
      playerPos.y += (baseSpeed * speedMultiplier);
    }

    player->JSGSetTranslation(playerPos);
  }
  return;
}

static u8 sHomeID = 0;
static u8 sTargetID = 254;
static TBGCheckData *sHomeTriangle;

void Patch::Debug::updateDebugCollision(TMario *player) {
  constexpr u32 SetHomeTriangleButtons =
      TMarioGamePad::EButtons::Z | TMarioGamePad::EButtons::DPAD_LEFT;
  constexpr u32 SetTargetTriangleButtons =
      TMarioGamePad::EButtons::Z | TMarioGamePad::EButtons::DPAD_RIGHT;

  if (!TGlobals::isDebugMode())
    return;

  const JUTGamePad::CButton &buttons = player->mController->mButtons;

  if (buttons.mFrameInput == SetHomeTriangleButtons) {
    sHomeTriangle = player->mFloorTriangle;
    sHomeID = (sHomeID + 1) % 255;
  } else if (buttons.mFrameInput == SetTargetTriangleButtons && sHomeTriangle) {
    if (sHomeTriangle == player->mFloorTriangle)
      return;

    sHomeTriangle->mCollisionType = 16042;
    sHomeTriangle->mValue4 = s16((sTargetID << 8) | (sHomeID - 1));
    player->mFloorTriangle->mCollisionType = 16042;
    player->mFloorTriangle->mValue4 = s16(((sHomeID - 1) << 8) | sTargetID);
    TGlobals::sWarpColArray->addLink(sHomeTriangle, player->mFloorTriangle);
    TGlobals::sWarpColArray->addLink(player->mFloorTriangle, sHomeTriangle);
    sTargetID = sTargetID != 0 ? (sTargetID - 1) : 254;
  }

  return;
}

static void setEmitPrm() {
  TWaterBalloon::sEmitInfo = new TWaterEmitInfo("/mario/waterballoon/waterballoon.prm");
  TParams::finalize();
}
SME_PATCH_BL(SME_PORT_REGION(0x802B8DC8, 0, 0, 0), setEmitPrm);

static u32 preventDebuggingDeath(TMario *player) {
  extern bool gInXYZMode;
  return gInXYZMode ? 0x224E0 : player->mState; // Spoof non dying value
};
SME_PATCH_BL(SME_PORT_REGION(0x80243110, 0, 0, 0), preventDebuggingDeath);

static void preventDebuggingInteraction(TMario *player, JDrama::TGraphics *graphics) {
  extern bool gInXYZMode;
  if (!gInXYZMode)
    player->playerControl(graphics);
  else {
    player->mForwardSpeed = 0.0f;
    player->mSpeed = {0.0f, 0.0f, 0.0f};
    player->mState = static_cast<u32>(TMario::State::IDLE);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x8024D3A0, 0, 0, 0), preventDebuggingInteraction);