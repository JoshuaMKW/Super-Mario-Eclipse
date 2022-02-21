#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

// extern -> SME.cpp
// 0x8025B8C0
static f32 checkGroundSpeedLimit() {
  TMario *player;
  SME_FROM_GPR(31, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  f32 multiplier = 1.0f;
  if (onYoshi__6TMarioCFv(player)) {
    multiplier *= player->mYoshiParams.mRunYoshiMult.get();
  } else {
    multiplier *= playerData->getParams()->mSpeedMultiplier.get();
  }
  return multiplier;
}
SME_WRITE_32(SME_PORT_REGION(0x8025B8BC, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8025B8C0, 0, 0, 0), checkGroundSpeedLimit);
SME_WRITE_32(SME_PORT_REGION(0x8025B8C4, 0, 0, 0), 0xEFFF0072);

static f32 checkSlideSpeedMulti() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  constexpr f32 speedCap = 100.0f;
  constexpr f32 rocketMultiplier = 1.8f;
  constexpr f32 hoverMultiplier = 1.2f;
  constexpr f32 brakeRate = 0.005f;

  if (player->mFludd && isEmitting__9TWaterGunFv(player->mFludd)) {
    if (player->mFludd->mCurrentNozzle == TWaterGun::Hover ||
        player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
      playerData->mSlideSpeedMultiplier = hoverMultiplier;
    else if (player->mFludd->mCurrentNozzle == TWaterGun::Rocket) {
      const f32 multiplier =
          (rocketMultiplier *
           ((speedCap * rocketMultiplier) / player->mForwardSpeed));
      playerData->mSlideSpeedMultiplier = rocketMultiplier;
      player->mPrevSpeed.scale(multiplier);
    } else {
      playerData->mSlideSpeedMultiplier =
          Max(playerData->mSlideSpeedMultiplier - brakeRate, 1.0f);
    }
  } else {
    playerData->mSlideSpeedMultiplier =
        Max(playerData->mSlideSpeedMultiplier - brakeRate, 1.0f);
  }

  if (!onYoshi__6TMarioCFv(player)) {
    return speedCap * playerData->mSlideSpeedMultiplier;
  } else {
    return speedCap;
  }
}
SME_WRITE_32(SME_PORT_REGION(0x8025C3D8, 0, 0, 0), 0x40810028);
SME_WRITE_32(SME_PORT_REGION(0x8025C3FC, 0, 0, 0), 0xFC800018);
SME_WRITE_32(SME_PORT_REGION(0x8025C400, 0, 0, 0), 0xD09E00B0);
SME_PATCH_BL(SME_PORT_REGION(0x8025C404, 0, 0, 0), checkSlideSpeedMulti);
SME_WRITE_32(SME_PORT_REGION(0x8025C408, 0, 0, 0), 0xFC400890);
SME_WRITE_32(SME_PORT_REGION(0x8025C410, 0, 0, 0), 0x60000000);

/* GOOP WALKING CODE */

// extern -> SME.cpp
// 0x8024E288
static void checkGraffitiAffected(TMario *player) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (!playerData->isMario()) {
    checkGraffito__6TMarioFv(player);
  } else if (playerData->getParams()->mGoopAffected.get()) {
    checkGraffito__6TMarioFv(player);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E288, 0, 0, 0), checkGraffitiAffected);