#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"
#include "libs/sGeometry.hxx"

using namespace SME;
using namespace SME::Util::Math;

// 0x80258334, 0x80264CFC
// extern -> SME.cpp
void Patch::Collision::checkIsGlideBounce(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16007 ||
      (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17007) {
    TBGCheckData *_oldCol = player->mFloorTriangle;
    u16 _oldType = _oldCol->mCollisionType;
    _oldCol->mCollisionType = 7;

    checkEnforceJump__6TMarioFv(player);
    _oldCol->mCollisionType = _oldType;

    playerData->mCollisionFlags.mIsSpinBounce = true;
    changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::JUMPSPIN1), 0, 0);
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
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if (playerData->mCollisionTimer <= 0) {
    incHP__6TMarioFi(player, 1);
    playerData->mCollisionTimer = player->mFloorTriangle->mValue4;
  } else
    playerData->mCollisionTimer -= 1;
}

static void checkIsCannonType(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if (!(player->mController->mButtons.mInput & TMarioGamePad::Buttons::DPAD_UP) ||
      !playerData->mCollisionFlags.mIsFaceUsed)
    return;

  if ((player->mFloorTriangle->mCollisionType & 0x7FFF) == 16080 ||
      (player->mFloorTriangle->mCollisionType & 0x7FFF) == 17080) {
    changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::TRIPLE_J), 0, 0);
    emitParticle__6TMarioFis(player, static_cast<u32>(TMario::Effect::GROUND_SHARP_SHOCK));
    emitParticle__6TMarioFis(player, static_cast<u32>(TMario::Effect::GROUND_SMOKE_PLUME));
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
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if (playerData->mCollisionFlags.mIsFaceUsed || !isMario__6TMarioFv(player))
    return;

  player->mAttributes.mHasFludd = player->mFloorTriangle->mValue4 == 1;

  TWaterGun::NozzleType nozzle = TWaterGun::Hover;
  if (type >= 17090)
    nozzle = static_cast<TWaterGun::NozzleType>(type - 17090);
  else
    nozzle = static_cast<TWaterGun::NozzleType>(type - 16090);

  changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(player->mFludd, nozzle,
                                                      1);
  playerData->mCollisionFlags.mIsFaceUsed = true;
}

static void boostPadCol(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  player->mForwardSpeed = player->mFloorTriangle->mValue4;
  if (player->mState == static_cast<u32>(TMario::State::IDLE) ||
      !playerData->mCollisionFlags.mIsFaceUsed) {
    changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::RUNNING), 0, 0);
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
    changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::FALL), 0, 0);
  }
}

static void warpToLinkedCol(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  TBGCheckData *linkedCol =
      SME::TGlobals::sWarpColArray->resolveCollisionWarp(
          player->mFloorTriangle);
  SME::Class::TVectorTriangle colTriangle(
      linkedCol->mVertexA, linkedCol->mVertexB, linkedCol->mVertexC);

  if (!linkedCol) {
    playerData->mCollisionTimer = 0;
    return;
  }

  const f32 speed = Vector::magnitude(player->mSpeed);

  if (speed > 1.0f) {
    playerData->mCollisionTimer = 0;
    return;
  }

  if (!playerData->mCollisionFlags.mIsFaceUsed) {
    if (playerData->mCollisionTimer > 140) {
      JGeometry::TVec3<f32> playerPos;
      JGeometry::TVec3<f32> cameraPos;
      player->JSGGetTranslation(reinterpret_cast<Vec *>(&playerPos));
      gpCamera->JSGGetViewPosition(reinterpret_cast<Vec *>(&cameraPos));

      playerPos.set(colTriangle.center());

      player->mFloorTriangle = linkedCol;
      player->mFloorTriangleCopy = linkedCol;
      player->mFloorBelow = playerPos.y;
      playerData->mCollisionFlags.mIsFaceUsed = true;
      playerData->mCollisionTimer = 0;
      startSoundActor__6TMarioFUl(player, static_cast<u32>(TMario::Voice::JUMP));

      {
        f32 x = SME::Util::Math::lerp<f32>(cameraPos.x, playerPos.x, 0.9375f);
        f32 y = playerPos.y + 300.0f;
        f32 z = SME::Util::Math::lerp<f32>(cameraPos.z, playerPos.z, 0.9375f);
        cameraPos.set(x, y, z);
      }
      gpCamera->mHorizontalAngle =
          static_cast<u16>(SME::Class::TVectorTriangle::bearingAngleY(
              playerPos, cameraPos)) *
          182;
    } else if (playerData->mCollisionTimer > 80) {
      playerData->mCollisionFlags.mIsDisableInput = true;
      player->mController->State.mReadInput = false;
      emitGetEffect__6TMarioFv(player);
    }
  } else {
    if (playerData->mCollisionTimer > 60) {
      playerData->mCollisionFlags.mIsDisableInput = false;
      player->mController->State.mReadInput = false;
    }
  }
  playerData->mCollisionTimer += 1;
}

static void warpToLinkedColPreserve(TMario *player, bool fluid) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  TBGCheckData *linkedCol =
      SME::TGlobals::sWarpColArray->resolveCollisionWarp(
          player->mFloorTriangle);
  SME::Class::TVectorTriangle colTriangle(
      linkedCol->mVertexA, linkedCol->mVertexB, linkedCol->mVertexC);

  if (!linkedCol)
    return;

  if (!playerData->mCollisionFlags.mIsFaceUsed) {
    JGeometry::TVec3<f32> playerPos;
    JGeometry::TVec3<f32> cameraPos;
    player->JSGGetTranslation(reinterpret_cast<Vec *>(&playerPos));
    gpCamera->JSGGetViewPosition(reinterpret_cast<Vec *>(&cameraPos));

    player->mFloorTriangle = linkedCol;
    player->mFloorTriangleCopy = linkedCol;
    playerData->mCollisionFlags.mIsFaceUsed = true;

    playerPos.set(colTriangle.center());

    {
      f32 x = SME::Util::Math::lerp<f32>(cameraPos.x, playerPos.x, 0.9375f);
      f32 y = playerPos.y + 300.0f;
      f32 z = SME::Util::Math::lerp<f32>(cameraPos.z, playerPos.z, 0.9375f);
      cameraPos.set(x, y, z);
    }
    gpCamera->mHorizontalAngle =
        static_cast<u16>(
            SME::Class::TVectorTriangle::bearingAngleY(playerPos, cameraPos)) *
        182;

    JGeometry::TVec3<f32> colNormal = colTriangle.normal();

    const f32 magnitude = fabsf(player->mSpeed.x) + fabsf(player->mSpeed.y) +
                          fabsf(player->mSpeed.z);

    player->mAngle.y =
        static_cast<u16>(Vector::getNormalAngle(colNormal)) * 182;
    setPlayerVelocity__6TMarioFf(player,
                                 magnitude * colNormal.x + magnitude * colNormal.z);
    player->mSpeed.y = magnitude * colNormal.y;

    playerPos.add(JGeometry::TVec3<f32>{40.0f * colNormal.x, 40.0f * colNormal.y,
                                        40.0f * colNormal.z});

    player->JSGSetTranslation(reinterpret_cast<Vec &>(playerPos));
    gpCamera->JSGSetViewPosition(reinterpret_cast<Vec &>(cameraPos));

    changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::FALL), 0, 0);
  } else
    playerData->mCollisionFlags.mIsFaceUsed =
        (!(player->mController->mButtons.mFrameInput &
             TMarioGamePad::Buttons::DPAD_DOWN) &&
         !fluid);
}

static inline void resetValuesOnStateChange(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

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
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if (!(player->mState & static_cast<u32>(TMario::State::AIRBORN))) {
    playerData->mCollisionFlags.mIsAirborn = false;
    playerData->mCollisionFlags.mIsDisableInput = false;
  }
}

static inline void resetValuesOnAirborn(TMario *player) {
  //SME::Class::TPlayerParams *playerData =
  //   SME::TGlobals::getPlayerParams(player);

  if (!(player->mState & static_cast<u32>(TMario::State::AIRBORN)))
    return;
}

static inline void resetValuesOnCollisionChange(TMario *player) {
  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);

  if (player->mFloorTriangle->mCollisionType !=
      playerData->mPrevCollisionType) {
    playerData->mPrevCollisionType = player->mFloorTriangle->mCollisionType;
    playerData->mCollisionFlags.mIsFaceUsed = false;
    playerData->mCollisionTimer = 0;
  }
}

// 0x802500B8
// extern -> SME.cpp
u32 Patch::Collision::updateCollisionContext(TMario *player) {
  resetValuesOnStateChange(player);
  resetValuesOnGroundContact(player);
  resetValuesOnAirborn(player);
  resetValuesOnCollisionChange(player);

  SME::Class::TPlayerParams *playerData =
      SME::TGlobals::getPlayerParams(player);
  const SME::Class::TCustomParams *params = playerData->getParams();

  f32 marioCollisionHeight = *(f32 *)0x80415CC4 * params->mSizeMultiplier.get();

  if (player->mCeilingAbove - player->mFloorBelow < marioCollisionHeight &&
      player->mRoofTriangle && !(player->mState & static_cast<u32>(TMario::State::AIRBORN))) {
    loserExec__6TMarioFv(player);
  }
  return 1;
}

// 0x80250CA0
// extern -> SME.cpp
u16 Patch::Collision::masterGroundCollisionHandler(TBGCheckData *colTriangle) {
  TMario *player;
  SME_FROM_GPR(29, player);

  u16 type = colTriangle->mCollisionType & 0x7FFF;
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
  case 16040:
  case 17040:
    warpToLinkedCol(player);
    break;
  case 16041:
  case 16042:
    warpToLinkedColPreserve(player, false);
    break;
  case 17041:
  case 17042:
    warpToLinkedColPreserve(player, true);
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
  }
  return type;
}

// 0x8025059C
// extern -> SME.cpp
u32 Patch::Collision::masterAllCollisionHandler(TMario *player) {
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