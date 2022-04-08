#include "JGeometry.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "MTX.h"
#include "SME.hxx"
#include "collision/WarpCollision.hxx"
#include "libs/sGeometry.hxx"

#if SME_EXTRA_COLLISION

using namespace SME;
using namespace SME::Util::Math;

static void elecPlayer(TMario *player, u8 flags) {
  if (player->mSubState == 0 && player->mState != 0x20338) {
    player->mHealth -= flags - 1;
    changePlayerStatus__6TMarioFUlUlb(player, 0x20338, 0, false);
  }
  if (gpMSound->gateCheck(0x1814)) {
    MSoundSE::startSoundActor(
        0x1814, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 4);
  }
  if (gpMSound->gateCheck(0x3806)) {
    MSoundSE::startSoundActor(
        0x3806, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 4);
  }
}

static void burnPlayer(TMario *player, u8 flags) {
  /*
   */

  changePlayerDropping__6TMarioFUlUl(player, 0x20464, 0);
  decHP__6TMarioFi(player, flags);

  dropObject__6TMarioFv(player);
  changePlayerStatus__6TMarioFUlUlb(player, 0x208B7, 1, false);
  player->mSpeed.y += 20.0f;
  emitAndBindToPosPtr__21TMarioParticleManagerFlPCQ29JGeometry8TVec3_f(
      *(u32 *)SME_PORT_REGION(0x8040E150, 0x80405818, 0, 0), 6,
      reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr);
  if (gpMSound->gateCheck(0x1813)) {
    MSoundSE::startSoundActor(
        0x1813, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 4);
  }
}

static void slipFloor(TMario *player, u8 flags) {
  const f32 strengthRun = Util::Math::scaleLinearAtAnchor<f32>(
      static_cast<f32>(flags), 0.001f, 1.0f);
  const f32 strengthSlide = Min(Util::Math::scaleLinearAtAnchor<f32>(
                                    static_cast<f32>(flags), 0.001f, 1.0f),
                                1.00001f);

#define SCALE_PARAM(param, scale) param.set(param.get() * scale)
  SCALE_PARAM(player->mDirtyParams.mBrakeStartValRun, strengthRun);
  SCALE_PARAM(player->mDirtyParams.mBrakeStartValSlip, strengthSlide);
  player->mDirtyParams.mPolSizeFootPrint.set(0.0f);
  player->mDirtyParams.mPolSizeJump.set(0.0f);
  player->mDirtyParams.mPolSizeRun.set(0.0f);
  player->mDirtyParams.mPolSizeSlip.set(0.0f);
  player->mDirtyParams.mDirtyMax.set(0.0f);
#undef SCALE_PARAM

  checkGraffitoSlip__6TMarioFv(player);
}

static void decHealth(TMario *player, u8 flags) {
  decHP__6TMarioFi(player, flags);
}

static void incHealth(TMario *player, u8 flags) {
  incHP__6TMarioFi(player, flags);
}

/* extern to handlers.cpp */

// Array of basic action functions bound to collision values
void (*gStateCBMap[])(TMario *player, u8 flags){
    elecPlayer, burnPlayer, slipFloor, decHealth, incHealth};
size_t gStateCBMapSize = sizeof(gStateCBMap) / sizeof(void *);

void checkIsGlideBounce(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16007 ||
      (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17007) {
    TBGCheckData *_oldCol = player->mFloorTriangle;
    u16 _oldType = _oldCol->mCollisionType;
    _oldCol->mCollisionType = 7;

    checkEnforceJump__6TMarioFv(player);
    _oldCol->mCollisionType = _oldType;

    playerData->mCollisionFlags.mIsSpinBounce = true;
    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::STATE_JUMPSPINR), 0, 0);
  } else
    checkEnforceJump__6TMarioFv(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x80258334, 0x802500C0, 0, 0), checkIsGlideBounce);
SME_PATCH_BL(SME_PORT_REGION(0x80264CFC, 0x8025CA88, 0, 0), checkIsGlideBounce);

u16 checkIsRestoreTypeNoFallDamage(TBGCheckData *floor) {
  if ((floor->mCollisionType & 0x7FFF) == 16010 ||
      (floor->mCollisionType & 0x7FFF) == 17010)
    return 10;
  else
    return floor->mCollisionType;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024C558, 0x802442E4, 0, 0),             checkIsRestoreTypeNoFallDamage);

void checkRestoreHealth(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if (playerData->mCollisionTimer <= 0) {
    incHP__6TMarioFi(player, 1);
    playerData->mCollisionTimer = player->mFloorTriangle->mValue4;
  } else
    playerData->mCollisionTimer -= 1;
}

void checkIsCannonType(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if (!(player->mController->mButtons.mInput &
        TMarioGamePad::EButtons::DPAD_UP) ||
      !playerData->mCollisionFlags.mIsFaceUsed)
    return;

  if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16080 ||
      (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17080) {
    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::STATE_TRIPLE_J), 0, 0);
    emitParticle__6TMarioFis(
        player, static_cast<u32>(TMario::EFFECT_GROUND_SHARP_SHOCK));
    emitParticle__6TMarioFis(
        player, static_cast<u32>(TMario::EFFECT_GROUND_SMOKE_PLUME));
    player->mForwardSpeed = (u8)(player->mFloorTriangle->mValue4 >> 8);

    {
      Vec position;
      player->JSGGetTranslation(&position);
      position.y += 1.0f;
      player->JSGSetTranslation(position);
    }

    player->mSpeed.y = (u8)player->mFloorTriangle->mValue4;
    playerData->mCollisionFlags.mIsDisableInput = true;
    player->mController->State.mReadInput = false;
    playerData->mCollisionFlags.mIsFaceUsed = true;
  }
}

void changeNozzleType(TMario *player, u16 type) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);
  TWaterGun *fludd = player->mFludd;

  if (playerData->mCollisionFlags.mIsFaceUsed || !fludd)
    return;

  if (!playerData->getCanUseFludd())
    return;

  player->mAttributes.mHasFludd = player->mFloorTriangle->mValue4 == 1;

  TWaterGun::NozzleType nozzle = TWaterGun::Hover;
  if (type >= 17090)
    nozzle = static_cast<TWaterGun::NozzleType>(type - 17090);
  else
    nozzle = static_cast<TWaterGun::NozzleType>(type - 16090);

  if (fludd->mCurrentNozzle != nozzle) {
    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(player->mFludd, nozzle,
                                                        true);
    emitGetEffect__6TMarioFv(player);
  } else if (fludd->mCurrentWater < fludd->mNozzleList[nozzle]->mMaxWater) {
    emitGetWaterEffect__6TMarioFv(player);
  }

  fludd->mCurrentWater = fludd->mNozzleList[nozzle]->mMaxWater;
  playerData->mCollisionFlags.mIsFaceUsed = true;
}

void boostPadCol(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  const f32 newSpeed = player->mFloorTriangle->mValue4;
  const f32 scale = newSpeed / player->mForwardSpeed;
  player->mForwardSpeed = newSpeed;
  player->mPrevSpeed.scale(scale);

  u32 targetState =
      (player->mState == static_cast<u32>(TMario::STATE_DIVESLIDE) ||
       player->mState == static_cast<u32>(TMario::STATE_GOOPSLIDE))
          ? player->mState
          : static_cast<u32>(TMario::STATE_RUNNING);
  if (player->mState == static_cast<u32>(TMario::STATE_IDLE) ||
      !playerData->mCollisionFlags.mIsFaceUsed) {
    changePlayerStatus__6TMarioFUlUlb(player, targetState, 0, 0);
    startVoice__6TMarioFUl(player, static_cast<u32>(TMario::VOICE_JUMP));
  }
}

#endif