#include "MTX.h"
#include "types.h"

//#include "sme/obj/WaterBalloon.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"


#include "SME.hxx"

using namespace SME;

bool gInXYZMode = false;

// extern c_kit.cpp
// 0x8024D194
void Patch::Debug::xyzModifierMario(TMario *player) {
  constexpr f32 baseSpeed = 83.0f;
  constexpr u32 buttons = TMarioGamePad::Buttons::DPAD_UP;

#ifndef SME_DEBUG
  if (!SME::Class::TCheatHandler::sDebugHandler.isActive())
    return;
#endif

  const JUTGamePad::CStick &mainStick = player->mController->mControlStick;
  const f32 speedMultiplier =
      SME::Util::Math::lerp<f32>(1, 2, player->mController->mButtons.mAnalogR);

  if (player->mController->mButtons.mFrameInput & buttons && !gInXYZMode) {
    setPlayerVelocity__6TMarioFf(player, 0.0f);
    player->mSpeed.y = 0.0f;

    gInXYZMode = true;
  } else if (player->mController->mButtons.mFrameInput & buttons &&
             gInXYZMode) {
    player->mState = static_cast<u32>(TMario::State::IDLE);
    gInXYZMode = false;
  }

  if (gInXYZMode) {
    const f32 cameraRotY = (f32)(gpCamera->mHorizontalAngle) / 182.0f;

    Vec playerPos;
    player->JSGGetTranslation(&playerPos);

    player->mState = static_cast<u32>(TMario::State::IDLE) |
                     static_cast<u32>(TMario::State::CUTSCENE);

    playerPos.x +=
        ((-sinf(SME::Util::Math::angleToRadians(cameraRotY)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickY;
    playerPos.z +=
        ((-cosf(SME::Util::Math::angleToRadians(cameraRotY)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickY;
    playerPos.x -=
        ((-sinf(SME::Util::Math::angleToRadians(cameraRotY + 90.0f)) *
          baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;
    playerPos.z -=
        ((-cosf(SME::Util::Math::angleToRadians(cameraRotY + 90.0f)) *
          baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;

    if (player->mController->mButtons.mInput & TMarioGamePad::Buttons::B) {
      playerPos.y -= (baseSpeed * speedMultiplier);
    } else if (player->mController->mButtons.mInput &
               TMarioGamePad::Buttons::A) {
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
      TMarioGamePad::Buttons::Z | TMarioGamePad::Buttons::DPAD_LEFT;
  constexpr u32 SetTargetTriangleButtons =
      TMarioGamePad::Buttons::Z | TMarioGamePad::Buttons::DPAD_RIGHT;

#ifndef SME_DEBUG
  if (!SME::Class::TCheatHandler::sDebugHandler.isActive())
    return;
#endif

  const JUTGamePad::CButton &buttons = player->mController->mButtons;

  if (buttons.mFrameInput & SetHomeTriangleButtons) {
    sHomeTriangle = player->mFloorTriangle;
    sHomeID = (sHomeID + 1) % 255;
  } else if (buttons.mFrameInput & SetTargetTriangleButtons && sHomeTriangle) {
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

void createWaterBalloonAndThrow(TMario *player) {
  return;
  /*
  TWaterBalloon *balloon = new TWaterBalloon("waterballoon");

  balloon->initAndRegister("waterballoon");
  balloon->awake();

  hold__14TMapObjGeneralFP10TTakeActor(balloon, player);
  if (player->mState == static_cast<u32>(TMario::State::IDLE)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x80000387, 0, 0);
  } else if (player->mState == static_cast<u32>(TMario::State::RUNNING)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 0);
  } else if (player->mState == static_cast<u32>(TMario::State::JUMP) ||
             player->mState == static_cast<u32>(TMario::State::D_JUMP) ||
             player->mState == static_cast<u32>(TMario::State::FALL)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x820008AB, 0, 0);
  }
  */
}