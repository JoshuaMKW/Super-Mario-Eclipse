#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

#if SME_HOVER_BURST

using namespace SME;

static bool sIsTriggerNozzleDead = false;

static void checkExecWaterGun(TWaterGun *fludd) {
  if (fludd->mCurrentNozzle == TWaterGun::NozzleType::Spray ||
      fludd->mCurrentNozzle == TWaterGun::NozzleType::Yoshi ||
      fludd->mCurrentNozzle == TWaterGun::NozzleType::Underwater) {
    emit__9TWaterGunFv(fludd);
    return;
  }

  if (!sIsTriggerNozzleDead)
    emit__9TWaterGunFv(fludd);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E548, 0x802462D4, 0, 0), checkExecWaterGun);

static void killTriggerNozzle() {
  TNozzleTrigger *nozzle;
  SME_FROM_GPR(29, nozzle);

  nozzle->mSprayState = TNozzleTrigger::DEAD;
  if (nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Hover ||
      nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Rocket ||
      nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Turbo)
    sIsTriggerNozzleDead = true;
}
SME_PATCH_BL(SME_PORT_REGION(0x8026C370, 0x802640FC, 0, 0), killTriggerNozzle);

// 0x80262580
// extern -> SME.cpp
static bool checkAirNozzle() {
  TMario *player;
  SME_FROM_GPR(31, player);

  sIsTriggerNozzleDead &=
      (player->mState & static_cast<u32>(TMario::State::AIRBORN)) != 0;

  if (player->mFludd->mCurrentNozzle == TWaterGun::Spray ||
      player->mFludd->mCurrentNozzle == TWaterGun::Yoshi ||
      player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
    return player->mState != static_cast<u32>(TMario::State::HOVER_F);

  return (!(player->mState & static_cast<u32>(TMario::State::AIRBORN)) ||
          !sIsTriggerNozzleDead);
}
SME_PATCH_BL(SME_PORT_REGION(0x80262580, 0x8025A30C, 0, 0), checkAirNozzle);
SME_WRITE_32(SME_PORT_REGION(0x80262584, 0x8025A310, 0, 0), 0x2C030000);

// extern -> fludd/generic.cpp
void checkSpamHover(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo) {
  TMario *player = nozzle->mFludd->mMario;
  Vec size;
  player->JSGGetScaling(&size);

  if (nozzle->mFludd->mCurrentNozzle != TWaterGun::Hover)
    return;

  nozzle->mEmitPerFrame = 1.0f;
  nozzle->mEmitRandom = 0.0f;

  if ((nozzle->mMaxSprayQuarterFrames - nozzle->mSprayQuarterFramesLeft) >= 20)
    return;

  if (!(player->mControllerWork->mAnalogR < 0.9f) ||
      !(player->mControllerWork->mFrameInput & TMarioControllerWork::A))
    return;

  if (nozzle->mFludd->mCurrentWater < 510)
    return;

  nozzle->mEmitPerFrame = 255.0f;
  nozzle->mEmitRandom = 0.5f;
  nozzle->mSprayQuarterFramesLeft = 0;
  nozzle->mSprayState = TNozzleTrigger::DEAD;

  nozzle->mFludd->mCurrentWater -= 255;
  player->mSpeed.y += (70.0f * size.y) - player->mSpeed.y;
  player->mJumpingState &= 0xFFFFFEFF;

  sIsTriggerNozzleDead = true;
  return;
}

#endif