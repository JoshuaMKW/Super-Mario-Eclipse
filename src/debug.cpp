#include "MTX.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

static bool gInXYZMode = false;

// extern SME.cpp
// 0x8024D194
bool Patch::Debug::xyzModifierMario(TMario *player) {
  constexpr f32 baseSpeed = 83.0f;

#ifndef SME_DEBUG
  if (!SME::Class::TCheatHandler::sDebugHandler.isActive())
    return;
#endif

  const JUTGamePad::CStick &mainStick = player->mController->mControlStick;
  const f32 speedMultiplier =
      SME::Util::Math::lerp<f32>(1, 2, player->mController->mButtons.mAnalogR);

  constexpr u32 mapInstrAddress = SME_PORT_REGION(0x80297A64, 0, 0, 0);

  if (gpMarDirector->mLastState == 0xA)
    return (player->_114 & 0x400) == 0x400;

  if (player->mController->mButtons.mFrameInput &
          TMarioGamePad::Buttons::DPAD_RIGHT &&
      !gInXYZMode) {
    setPlayerVelocity__6TMarioFf(player, 0.0f);
    player->mSpeed.y = 0.0f;

    gInXYZMode = true;
  } else if (player->mController->mButtons.mFrameInput &
                 TMarioGamePad::Buttons::DPAD_RIGHT &&
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
        ((-sinf(SME::Util::Math::angleToRadians(cameraRotY+90.0f)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;
    playerPos.z -=
        ((-cosf(SME::Util::Math::angleToRadians(cameraRotY+90.0f)) * baseSpeed) *
         speedMultiplier) *
        mainStick.mStickX;

    if (player->mController->mButtons.mInput &
        TMarioGamePad::Buttons::B) {
      playerPos.y -= (baseSpeed * speedMultiplier);
    } else if (player->mController->mButtons.mInput &
               TMarioGamePad::Buttons::A) {
      playerPos.y += (baseSpeed * speedMultiplier);
    }

    player->JSGSetTranslation(playerPos);
  }
  return (player->_114 & 0x400) == 0x400;
}