#include "types.h"

#include "sms/actor/Mario.hxx"
#include "SME.hxx"

using namespace SME;

f32 calcJumpPower(TMario *player, f32 factor, f32 base, f32 jumpPower) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  const Class::TPlayerParams *params = playerData->getParams();

  jumpPower *= params->mBaseJumpMultiplier.get();
  jumpPower *= Util::Math::scaleLinearAtAnchor<f32>(
      params->mSizeMultiplier.get() *
          Class::TStageParams::sStageConfig->mPlayerSizeMultiplier.get(),
      0.5f, 1.0f);
  if (player->mState & static_cast<u32>(TMario::State::AIRBORN)) {
    f32 multiplier = params->mMultiJumpMultiplier.get();
    for (u32 i = 1; i < playerData->mCurJump; ++i) {
      multiplier *= multiplier;
    }
    jumpPower *= multiplier;
    player->mForwardSpeed *= params->mMultiJumpFSpeedMulti.get();
  }
  return Max(base, (base * factor) + jumpPower);
}

#if SME_LONG_JUMP

static void setJumpOrLongJump(TMario *player, u32 state, u32 unk_0) {
  constexpr u32 LongJumpSpecifier = TMarioGamePad::EButtons::Z;
  constexpr f32 LongJumpMinSpeed = 10.0f;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TMarioGamePad::CButton &buttons = player->mController->mButtons;

  const bool isValidState =
      !(player->mState & static_cast<u32>(TMario::State::AIRBORN)) &&
      !(player->mState & static_cast<u32>(TMario::State::WATERBORN));

  playerData->mIsLongJumping = false;
  if ((buttons.mInput & LongJumpSpecifier) == LongJumpSpecifier &&
      (buttons.mFrameInput & TMarioGamePad::EButtons::A) &&
      player->mForwardSpeed > LongJumpMinSpeed && isValidState) {
    playerData->mIsLongJumping = !player->mAttributes.mHasFludd &&
                                 playerData->isMario() &&
                                 (player->mActionState & 0x8) == 0;
    state = static_cast<u32>(TMario::State::JUMP);
  }

  setStatusToJumping__6TMarioFUlUl(player, state, unk_0);
}
SME_PATCH_BL(SME_PORT_REGION(0x802541BC, 0x8024BF48, 0, 0), setJumpOrLongJump);

static void processJumpOrLongJumpAnim(TMario *player, int state, int anim,
                                      int unk_0) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->mIsLongJumping) {
    anim = 0xF6;
  }

  jumpingBasic__6TMarioFiii(player, state, anim, unk_0);
}
SME_PATCH_BL(SME_PORT_REGION(0x80249554, 0x802412E0, 0, 0), processJumpOrLongJumpAnim);

static void processJumpOrLongJump() {
  TMario *player;
  SME_FROM_GPR(30, player);

  constexpr f32 LongJumpSpeedForward = 36.0f;
  constexpr f32 LongJumpSpeedUp = 50.0f;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!playerData->mIsLongJumping) {
    player->mSpeed.y =
        calcJumpPower(player, 0.25f, player->mForwardSpeed, 42.0f);
    return;
  }

  startVoice__6TMarioFUl(player, TMario::Voice::TRIPLE_JUMP);

  player->mSpeed.y +=
      LongJumpSpeedUp * playerData->getParams()->mBaseJumpMultiplier.get();
  player->mForwardSpeed +=
      LongJumpSpeedForward * playerData->getParams()->mSpeedMultiplier.get();
  player->mPrevState = player->mState;
  player->mState = static_cast<u32>(TMario::State::JUMP);
}
SME_PATCH_BL(SME_PORT_REGION(0x80254534, 0x8024c2c0, 0, 0), processJumpOrLongJump);
SME_WRITE_32(SME_PORT_REGION(0x80254538, 0x8024c2c4, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025453C, 0x8024c2c8, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254540, 0x8024c2cc, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254544, 0x8024c2d0, 0, 0), 0x60000000);

static bool checkDivingWhenLongJumping(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return onYoshi__6TMarioCFv(player) || playerData->mIsLongJumping;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C394, 0x80244120, 0, 0), checkDivingWhenLongJumping);

static bool checkRotatingWhenLongJumping(TMario *player, int *unk_0) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return checkStickRotate__6TMarioFPi(player, unk_0) &&
         !playerData->mIsLongJumping;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C3F8, 0x80244184, 0, 0),             checkRotatingWhenLongJumping);

static bool checkQuickFallWhenLongJumping() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  return ((player->mActionState & 0x80) != 0) || playerData->mIsLongJumping ||
         playerData->mCollisionFlags.mIsSpinBounce;
}
SME_PATCH_BL(SME_PORT_REGION(0x802565D4, 0x8024E360, 0, 0),             checkQuickFallWhenLongJumping);
SME_WRITE_32(SME_PORT_REGION(0x802565D8, 0x8024E364, 0, 0), 0x2C030000);

static bool preserveRegisterCheckQuickFall() {
  TMario *player;
  SME_FROM_GPR(31, player);

  return player->mState == static_cast<u32>(TMario::State::JUMPSPINR) ||
         player->mState == static_cast<u32>(TMario::State::JUMPSPINL);
}
SME_PATCH_BL(SME_PORT_REGION(0x80256618, 0x8024E3A4, 0, 0),             preserveRegisterCheckQuickFall);
SME_WRITE_32(SME_PORT_REGION(0x8025661C, 0x8024E3A8, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x80256620, 0x8024E3AC, 0, 0), 0x41820024);

#endif