#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

static void doProcessJumpState(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TMarioGamePad *controller = player->mController;

  const f32 stickMagnitude = controller->mControlStick.mLengthFromNeutral;

#if 1
  if (player->mState != static_cast<u32>(TMario::State::JUMPSPINR) &&
      player->mState != static_cast<u32>(TMario::State::JUMPSPINL))
    playerData->mCollisionFlags.mIsSpinBounce = false;

  if (playerData->mCollisionFlags.mIsSpinBounce) {
    if (stickMagnitude > 0.1f) {
      Util::Mario::rotatePlayerRelativeToCamera(
          player, gpCamera,
          {controller->mControlStick.mStickX,
           controller->mControlStick.mStickY},
          1.0f);
    } else {
      player->mForwardSpeed *= 0.98f;
    }
  }
#else
  Util::Mario::rotatePlayerRelativeToCamera(
      player, gpCamera,
      {controller->mControlStick.mStickX, controller->mControlStick.mStickY},
      1.0f);
#endif

  jumpMain__6TMarioFv(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x80250138, 0, 0, 0), doProcessJumpState);

static void checkYSpdForTerminalVelocity() {
  TMario *player;
  SME_FROM_GPR(31, player);

  float terminalVelocity;
  if (TGlobals::getPlayerData(player)->mCollisionFlags.mIsSpinBounce)
    terminalVelocity = -20.0f * player->mJumpParams.mGravity.get();
  else
    terminalVelocity = -75.0f * player->mJumpParams.mGravity.get();
  player->mSpeed.y = Max(player->mSpeed.y, terminalVelocity);
}
SME_PATCH_BL(SME_PORT_REGION(0x80256678, 0, 0, 0),
             checkYSpdForTerminalVelocity);
SME_WRITE_32(SME_PORT_REGION(0x8025667C, 0, 0, 0), 0x60000000);

static void checkJumpSpeedLimit(f32 speed) {
  TMario *player;
  SME_FROM_GPR(31, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  f32 speedCap = 32.0f;
  f32 speedReducer = 0.2f;

  if (!onYoshi__6TMarioCFv(player)) {
    speedCap *= playerData->getParams()->mSpeedMultiplier.get();
    speedReducer *= Util::Math::scaleLinearAtAnchor<f32>(
        playerData->getParams()->mSpeedMultiplier.get(), 3.0f, 1.0f);
  }

  if (speed > speedCap) {
    player->mForwardSpeed = (speed - speedReducer);
  }
}
SME_WRITE_32(SME_PORT_REGION(0x8024CB00, 0, 0, 0), 0xC162EF70);
SME_WRITE_32(SME_PORT_REGION(0x8024CC50, 0, 0, 0), 0xED600072);
SME_WRITE_32(SME_PORT_REGION(0x8024CC60, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8024CC64, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8024CC68, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8024CC6C, 0, 0, 0), checkJumpSpeedLimit);

static TMario *checkJumpSpeedMulti(TMario *player, f32 factor, f32 max) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (playerData->isMario() && !onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        ((factor * playerData->getParams()->mSpeedMultiplier.get()) * max) +
        player->mForwardSpeed;
    return player;
  } else {
    player->mForwardSpeed = (factor * max) + player->mForwardSpeed;
    return player;
  }
}
SME_WRITE_32(SME_PORT_REGION(0x8024CCF8, 0, 0, 0), 0xEC0B007A);
SME_WRITE_32(SME_PORT_REGION(0x8024CD24, 0, 0, 0), 0xEC0B007A);
SME_PATCH_BL(SME_PORT_REGION(0x8024CC2C, 0, 0, 0), checkJumpSpeedMulti);
SME_WRITE_32(SME_PORT_REGION(0x8024CC30, 0, 0, 0), 0x57C5043E);

static void dynamicFallDamage(TMario *player, int dmg, int type, int emitcount,
                              int tremble) {
#define floorDamageExec__6TMarioFiiii                                          \
  ((void (*)(TMario *, int, int, int, int))SME_PORT_REGION(0x8024303C, 0, 0, 0))

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  dmg *= static_cast<int>((player->mLastGroundedPos.y - player->mPosition.y) /
                          (player->mDeParams.mDamageFallHeight.get() / 1.4f));
  if (dmg > 2) {
    type = 0; // shaky
    emitcount = (dmg - 2) * 3;
  }

  if (player->mSpeed.y <= (-75.0f * player->mJumpParams.mGravity.get()) + 5.0f)
    floorDamageExec__6TMarioFiiii(player, dmg, type, emitcount, tremble);

#undef floorDamageExec__6TMarioFiiii
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C73C, 0, 0, 0), dynamicFallDamage);

// 8024afe0 <- hover air Y spd

static SME_PURE_ASM void scaleFluddInitYSpd() {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xFE0, 0, 0, 0)) "(2)  \n\t"
                        "lfs 4, 0x28(30)                          \n\t"
                        "fmuls 0, 0, 4                            \n\t"
                        "blr                                      \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80254A2C, 0, 0, 0), scaleFluddInitYSpd);