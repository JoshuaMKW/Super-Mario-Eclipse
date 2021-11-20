#include "JGeometry.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/sound/MSoundSESystem.hxx"
#include "sms/sound/MSound.hxx"

#include "MTX.h"
#include "SME.hxx"
#include "libs/sGeometry.hxx"

using namespace SME;
using namespace SME::Util::Math;

/*
static f32 checkGroundWithFlag(TMario *player, f32 x, f32 y, f32 z,
                               TBGCheckData **dst) {
  // gpMap->
  return 0.0f;
}
// SME_PATCH_BL(SME_PORT_REGION(0x802510E4, 0, 0, 0), checkGroundWithFlag);
*/

// 0x80258334, 0x80264CFC
// extern -> SME.cpp
void Patch::Collision::checkIsGlideBounce(TMario *player) {
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
        player, static_cast<u32>(TMario::State::JUMPSPIN1), 0, 0);
  } else
    checkEnforceJump__6TMarioFv(player);
}

// 0x8024C558
// extern -> SME.cpp
u16 Patch::Collision::checkIsRestoreTypeNoFallDamage(TBGCheckData *floor) {
  if ((floor->mCollisionType & 0x7FFF) == 16010 ||
      (floor->mCollisionType & 0x7FFF) == 17010)
    return 10;
  else
    return floor->mCollisionType;
}

static void checkRestoreHealth(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if (playerData->mCollisionTimer <= 0) {
    incHP__6TMarioFi(player, 1);
    playerData->mCollisionTimer = player->mFloorTriangle->mValue4;
  } else
    playerData->mCollisionTimer -= 1;
}

static void checkIsCannonType(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if (!(player->mController->mButtons.mInput &
        TMarioGamePad::Buttons::DPAD_UP) ||
      !playerData->mCollisionFlags.mIsFaceUsed)
    return;

  if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16080 ||
      (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17080) {
    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::State::TRIPLE_J), 0, 0);
    emitParticle__6TMarioFis(
        player, static_cast<u32>(TMario::Effect::GROUND_SHARP_SHOCK));
    emitParticle__6TMarioFis(
        player, static_cast<u32>(TMario::Effect::GROUND_SMOKE_PLUME));
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

static void changeNozzleType(TMario *player, u16 type) {
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

static void boostPadCol(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  const f32 newSpeed = player->mFloorTriangle->mValue4;
  const f32 scale = newSpeed / player->mForwardSpeed;
  player->mForwardSpeed = newSpeed;
  player->mPrevSpeed.scale(scale);

  u32 targetState =
      (player->mState == static_cast<u32>(TMario::State::DIVESLIDE) ||
       player->mState == static_cast<u32>(TMario::State::GOOPSLIDE))
          ? player->mState
          : static_cast<u32>(TMario::State::RUNNING);
  if (player->mState == static_cast<u32>(TMario::State::IDLE) ||
      !playerData->mCollisionFlags.mIsFaceUsed) {
    changePlayerStatus__6TMarioFUlUlb(player, targetState, 0, 0);
    startVoice__6TMarioFUl(player, static_cast<u32>(TMario::Voice::JUMP));
  }
}

static void setGravityCol(TMario *player) {
  player->mJumpParams.mGravity.set(
      static_cast<f32>(player->mFloorTriangle->mValue4) / 100.0f);
}

static void antiGravityCol(TMario *player) {
  Vec position;
  player->JSGGetTranslation(&position);

  if ((position.y - player->mFloorTriangle->mMaxHeight) >
          player->mFloorTriangle->mValue4 &&
      player->mFloorTriangle->mValue4 != 0)
    return;

  player->mSpeed.y = 10.0f;
  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == false) {
    position.y += 1.0f;
    player->JSGSetTranslation(position);
    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::State::FALL), 0, 0);
  }
  if (player->mState == static_cast<u32>(TMario::State::FALL))
    player->mSubStateTimer = 0;
}

static bool sIsWiping = false;

static SME_NO_INLINE void warpToLinkedCol(TMario *player, Enum::WarpKind kind) {
  constexpr s32 DisableMovementTime = 80;
  constexpr s32 TeleportTime = 140;
  constexpr s32 EnableMovementTime = 60;
  constexpr f32 WipeKindInDelay = 1.0f;

  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  TBGCheckData *linkedCol = SME::TGlobals::sWarpColArray->resolveCollisionWarp(
      player->mFloorTriangle);

  const f32 speed = PSVECMag(reinterpret_cast<Vec *>(&player->mSpeed));

  if (playerData->mCollisionFlags.mIsWarpActive) {
    switch (playerData->mCollisionFlags.mWarpingType) {
    case Enum::WarpKind::SPARKLES: {
      if (playerData->mCollisionTimer > EnableMovementTime) {
        playerData->mCollisionFlags.mIsDisableInput = false;
        playerData->mCollisionFlags.mIsWarpActive = false;
        player->mController->State.mReadInput = true;
        playerData->mCollisionTimer = 0;
      } else {
        playerData->mCollisionTimer += 1;
      }
      break;
    }
    case Enum::WarpKind::WIPE: {
      if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_OFF) {
        playerData->mCollisionFlags.mIsDisableInput = false;
        playerData->mCollisionFlags.mIsWarpActive = false;
        player->mController->State.mReadInput = true;
        playerData->mCollisionTimer = 0;
      } else {
        playerData->mCollisionTimer += 1;
      }
      break;
    }
    case Enum::WarpKind::INSTANT:
    default:
      playerData->mCollisionFlags.mIsDisableInput = false;
      playerData->mCollisionFlags.mIsWarpActive = false;
    }
  } else {
    if (!linkedCol) {
      playerData->mCollisionTimer = 0;
      return;
    }

    switch (kind) {
    case Enum::WarpKind::SPARKLES: {
      if (speed > 1.0f) {
        playerData->mCollisionTimer = 0;
        return;
      }

      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        if (playerData->mCollisionTimer > TeleportTime) {
          Util::Mario::warpToCollisionFace(player, linkedCol, false);

          playerData->mCollisionFlags.mWarpingType = kind;
          playerData->mCollisionFlags.mIsFaceUsed = true;
          playerData->mCollisionFlags.mIsWarpActive = true;
          playerData->mCollisionFlags.mIsWarpUsed = true;
          playerData->mCollisionTimer = 0;
          startSoundActor__6TMarioFUl(player,
                                      static_cast<u32>(TMario::Voice::JUMP));
        } else if (playerData->mCollisionTimer > DisableMovementTime) {
          if (!playerData->mCollisionFlags.mIsDisableInput) {
            emitGetEffect__6TMarioFv(player);
          }
          playerData->mCollisionFlags.mIsDisableInput = true;
          player->mController->State.mReadInput = false;
        }
      }
      playerData->mCollisionTimer += 1;
      return;
    }
    case Enum::WarpKind::WIPE: {
      if (speed > 1.0f) {
        playerData->mCollisionTimer = 0;
        return;
      }

      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_ON) {
          Util::Mario::warpToCollisionFace(player, linkedCol, false);

          playerData->mCollisionFlags.mWarpingType = kind;
          playerData->mCollisionFlags.mIsFaceUsed = true;
          playerData->mCollisionFlags.mIsWarpActive = true;
          playerData->mCollisionFlags.mIsWarpUsed = true;
          playerData->mCollisionTimer = 0;
          sIsWiping = false;

          gpApplication.mFader->startWipe(
              TSMSFader::WipeRequest::FADE_CIRCLE_IN, 1.0f, WipeKindInDelay);
        } else if (playerData->mCollisionTimer > DisableMovementTime) {
          playerData->mCollisionFlags.mIsDisableInput = true;
          player->mController->State.mReadInput = false;
          if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_OFF &&
              !sIsWiping) {
            gpApplication.mFader->startWipe(
                TSMSFader::WipeRequest::FADE_SPIRAL_OUT, 1.0f, 0.0f);
            MSoundSESystem::MSoundSE::startSoundSystemSE(0x4859, 0, nullptr, 0);
            sIsWiping = true;
          }
        }
      }
      playerData->mCollisionTimer += 1;
      return;
    }
    case Enum::WarpKind::INSTANT:
    default: {
      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        Util::Mario::warpToCollisionFace(player, linkedCol, false);

        playerData->mCollisionFlags.mWarpingType = kind;
        playerData->mCollisionFlags.mIsFaceUsed = true;
        playerData->mCollisionFlags.mIsWarpUsed = true;
        playerData->mCollisionTimer = 0;
      }
      return;
    }
    }
  }
}

static void warpToLinkedColPreserve(TMario *player, bool fluid) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  TBGCheckData *linkedCol = SME::TGlobals::sWarpColArray->resolveCollisionWarp(
      player->mFloorTriangle);

  if (!linkedCol)
    return;

  if (!playerData->mCollisionFlags.mIsFaceUsed) {
    Util::Mario::warpToCollisionFace(player, linkedCol, true);
  } else {
    playerData->mCollisionFlags.mIsFaceUsed =
        (!(player->mController->mButtons.mFrameInput &
           TMarioGamePad::Buttons::DPAD_DOWN) &&
         !fluid);
  }
}

#define EXPAND_WARP_SET(base)                                                  \
  (base) : case ((base) + 10) : case ((base) + 20) : case ((base) + 30)
#define EXPAND_WARP_CATEGORY(base)                                             \
  (base)                                                                       \
      : case ((base) + 1)                                                      \
      : case ((base) + 2) : case ((base) + 3) : case ((base) + 4)

static inline void resetValuesOnStateChange(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  switch (player->mPrevState) {
  case static_cast<u32>(TMario::State::JUMPSPIN1):
    playerData->mCollisionFlags.mIsSpinBounce = false;
  case static_cast<u32>(TMario::State::TRIPLE_J):
    playerData->mCollisionFlags.mIsDisableInput = false;
    player->mController->State.mReadInput = true;
  }
  if (playerData->mCollisionFlags.mIsDisableInput)
    // Patches pausing/map escaping the controller lock
    player->mController->State.mReadInput = false;
}

static inline void resetValuesOnGroundContact(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if ((player->mPrevState & static_cast<u32>(TMario::State::AIRBORN)) != 0 &&
      (player->mState & static_cast<u32>(TMario::State::AIRBORN)) == 0 &&
      playerData->mCollisionFlags.mIsAirborn) {
    playerData->mCollisionFlags.mIsAirborn = false;
    playerData->mCollisionFlags.mIsDisableInput = false;
  }
}

static inline void resetValuesOnAirborn(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if ((player->mPrevState & static_cast<u32>(TMario::State::AIRBORN)) == 0 &&
      (player->mState & static_cast<u32>(TMario::State::AIRBORN)) != 0 &&
      !playerData->mCollisionFlags.mIsAirborn) {
    playerData->mCollisionFlags.mIsAirborn = true;
  }
}

static inline void resetValuesOnCollisionChange(TMario *player) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  if (!player->mFloorTriangle ||
      (player->mFloorTriangle == playerData->mPrevCollisionFloor))
    return;

  if (player->mFloorTriangle->mCollisionType !=
      playerData->mPrevCollisionType) {
    playerData->mPrevCollisionType = player->mFloorTriangle->mCollisionType;
    playerData->mPrevCollisionFloor = player->mFloorTriangle;
    playerData->mCollisionFlags.mIsFaceUsed = false;
    playerData->mCollisionTimer = 0;
  }

  switch (player->mFloorTriangle->mCollisionType) {
  case EXPAND_WARP_SET(16042):
  case EXPAND_WARP_SET(17042):
    playerData->setColliding(false);
    break;
  default:
    playerData->setColliding(true);
    break;
  }
}

// 0x802500B8
// extern -> SME.cpp
u32 Patch::Collision::updateCollisionContext(TMario *player) {
  constexpr s16 CrushTimeToDie = 0;

  resetValuesOnStateChange(player);
  resetValuesOnGroundContact(player);
  resetValuesOnAirborn(player);
  resetValuesOnCollisionChange(player);

  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  const f32 marioCollisionHeight =
      *(f32 *)SME_PORT_REGION(0x80415CC4, 0, 0, 0) *
      playerData->getParams()->mSizeMultiplier.get();

  Vec playerPos;
  player->JSGGetTranslation(&playerPos);

  TBGCheckData *roofTri;

  f32 roofHeight = checkRoofPlane__6TMarioFRC3VecfPPC12TBGCheckData(
      player, playerPos, playerPos.y + (marioCollisionHeight / 4), &roofTri);

  if (!player->mAttributes.mIsGameOver) {
    if (roofHeight - player->mFloorBelow < (marioCollisionHeight - 40.0f) &&
        !(player->mState & static_cast<u32>(TMario::State::AIRBORN)) &&
        player->mState != static_cast<u32>(TMario::State::HANG) &&
        !isUnderWater__6TMarioCFv(player)) {
      playerData->mCollisionFlags.mCrushedTimer += 1;
    } else {
      playerData->mCollisionFlags.mCrushedTimer = 0;
    }

    if (playerData->mCollisionFlags.mCrushedTimer > CrushTimeToDie) {
      loserExec__6TMarioFv(player);
      playerData->mCollisionFlags.mCrushedTimer = 0;
    }
  }

  return 1;
}

static void electrifyPlayer(TMario *player) {
  changePlayerStatus__6TMarioFUlUlb(player, 0x20338, 0, false);
  if (gpMSound->gateCheck(0x1814)) {
    MSoundSESystem::MSoundSE::startSoundActor(0x1814, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 4);
  }
  if (gpMSound->gateCheck(0x3806)) {
    MSoundSESystem::MSoundSE::startSoundActor(0x3806, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 4);
  }
}

// 0x80250C9C
// extern -> SME.cpp
static TBGCheckData *masterGroundCollisionHandler() {
  TMario *player;
  SME_FROM_GPR(29, player);

  TBGCheckData *floorCol = player->mFloorTriangle;
  const u16 type = floorCol->mCollisionType & 0x7FFF;
  switch (type) {
  case 16007:
  case 17007:
    checkRestoreHealth(player);
    break;
  case 16010:
  case 17010:
    checkRestoreHealth(player);
    break;
  case 16020:
  case 17020:
    boostPadCol(player);
    break;
  case 16021:
  case 17021:
    setGravityCol(player);
    break;
  case EXPAND_WARP_SET(16040):
  case EXPAND_WARP_SET(17040):
    warpToLinkedCol(player, Enum::WarpKind::SPARKLES);
    break;
  case EXPAND_WARP_SET(16041):
  case EXPAND_WARP_SET(17041):
    warpToLinkedColPreserve(player, false);
    break;
  case EXPAND_WARP_SET(16042):
  case EXPAND_WARP_SET(17042):
    warpToLinkedColPreserve(player, true);
    break;
  case EXPAND_WARP_SET(16043):
  case EXPAND_WARP_SET(17043):
    warpToLinkedCol(player, Enum::WarpKind::WIPE);
    break;
  case EXPAND_WARP_SET(16044):
  case EXPAND_WARP_SET(17044):
    warpToLinkedCol(player, Enum::WarpKind::INSTANT);
    break;
  case 16080:
  case 17080:
    checkIsCannonType(player);
    break;
  case 16090:
  case 16091:
  case 16092:
  case 16093:
  case 16094:
  case 16095:
  case 17090:
  case 17091:
  case 17092:
  case 17093:
  case 17094:
  case 17095:
    changeNozzleType(player, type);
    break;
  case 16081:
    electrifyPlayer(player);
  }
  return floorCol;
}
SME_PATCH_BL(SME_PORT_REGION(0x80250C9C, 0, 0, 0), masterGroundCollisionHandler);

// 0x8025059C
// extern -> SME.cpp
static u32 masterAllCollisionHandler(TMario *player) {
  u16 type = player->mFloorTriangle->mCollisionType & 0x7FFF;
  switch (type) {
  case 16022:
  case 17022:
    setGravityCol(player);
    break;
  case 16023:
  case 17023:
    antiGravityCol(player);
    break;
  case 16190:
  case 16191:
  case 16192:
  case 16193:
  case 16194:
  case 16195:
  case 17190:
  case 17191:
  case 17192:
  case 17193:
  case 17194:
  case 17195:
    changeNozzleType(player, type);
    break;
  }
  return player->mState;
}
SME_PATCH_BL(SME_PORT_REGION(0x8025059C, 0, 0, 0),
             masterAllCollisionHandler);
SME_WRITE_32(SME_PORT_REGION(0x802505A0, 0, 0, 0), 0x546004E7);

#undef EXPAND_WARP_SET
#undef EXPAND_WARP_CATEGORY