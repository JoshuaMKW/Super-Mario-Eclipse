#include "types.h"

#include "sms/actor/Mario.hxx"
#include "SME.hxx"

#if SME_MULTI_JUMP

using namespace SME;

extern f32 calcJumpPower(TMario *player, f32 factor, f32 base, f32 jumpPower);

// extern -> SME.cpp
// 0x8024E02C
static void manageCustomJumps(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  const s32 jumpsLeft =
      (playerData->getParams()->mMaxJumps.get() - playerData->mCurJump);

  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == false ||
      (player->mState & 0x800000) ||
      player->mYoshi->mState == TYoshi::MOUNTED ||
      player->mState == static_cast<u32>(TMario::State::SLIP_JUMP) ||
      player->mState == static_cast<u32>(TMario::State::THROWN) ||
      player->mAttributes.mIsGameOver) {
    playerData->mCurJump = 1;
  } else if ((player->mController->mButtons.mFrameInput &
              TMarioGamePad::EButtons::A) &&
             jumpsLeft > 0 &&
             player->mState != static_cast<u32>(TMario::State::WALLSLIDE) &&
             player->mState != static_cast<u32>(TMario::State::F_KNCK_H)) {
    u32 state = player->mState;
    u32 voiceID = 0;
    u32 animID = 0;

    if (jumpsLeft == 1) {
      state = static_cast<u32>(TMario::State::TRIPLE_J);
      voiceID = 0x78B6;
      animID = 0x6F;
    } else if (jumpsLeft % 2) {
      state = static_cast<u32>(TMario::State::JUMP);
      voiceID = 0x78B1;
      animID = 0x4D;
    } else {
      state = static_cast<u32>(TMario::State::D_JUMP);
      voiceID = 0x78B6;
      animID = 0x50;
    }

    startVoice__6TMarioFUl(player, voiceID);
    setAnimation__6TMarioFif(player, animID, 1.0f);

    TMarioGamePad *controller = player->mController;
    const f32 stickMagnitude = controller->mControlStick.mLengthFromNeutral;

    if (stickMagnitude > 0.1f) {
      Util::Mario::rotatePlayerRelativeToCamera(
          player, gpCamera,
          {controller->mControlStick.mStickX,
           controller->mControlStick.mStickY}, 1.0f);
    }

    player->mForwardSpeed *= stickMagnitude;
    player->mSpeed.y = calcJumpPower(player, 0.25f, player->mSpeed.y, 65.0f);
    player->mPrevState = player->mState;
    player->mState = state;

    playerData->mIsLongJumping = false;
    playerData->mCurJump += 1;
  }
  stateMachine__6TMarioFv(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E02C, 0, 0, 0), manageCustomJumps);

#endif