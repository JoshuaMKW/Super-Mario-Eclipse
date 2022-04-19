#include "sms/SMS.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/manager/EffectObjManager.hxx"
#include "sms/object/EffectObjBase.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "Globals.hxx"
#include "libs/sLogging.hxx"
#include "params/MarioParams.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

static constexpr s32 MaxFireDamageTime = 300;
static constexpr s32 MaxFireTime = MaxFireDamageTime * 3;

namespace SME::Util::Mario {

void setFireToPlayer(TMario *player) {
  TPlayerData *params = TGlobals::getPlayerData(player);
  if (params->mIsOnFire) {
    params->mFireTimer %= MaxFireDamageTime;
    return;
  }
  params->mIsOnFire = true;
  params->mFireTimer = 0;
  params->mFireTimerMax = MaxFireTime;
  changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 1);
}

void extinguishPlayer(TMario *player, bool died) {
  TPlayerData *params = TGlobals::getPlayerData(player);

  if (params->mIsOnFire && !died)
    MSoundSE::startSoundActor(
        0x28C5, reinterpret_cast<Vec *>(&player->mPosition), 0, nullptr, 0, 0);

  params->mIsOnFire = false;
  params->mFireTimer = 0;
}

} // namespace SME::Util::Mario

void blazePlayer(TMario *player) {
  TPlayerData *params = TGlobals::getPlayerData(player);

  const f32 fireScale = 3.0f - (static_cast<f32>(params->mFireTimer) / MaxFireTime) * 2.0f;

  JPABaseEmitter *emitterFire = gpMarioParticleManager->emitAndBindToPosPtr(
      0x135, &player->mPosition, 1, player);
  emitterFire->mSize2.set(player->mSize.x * fireScale, player->mSize.y * fireScale,
                         player->mSize.z * fireScale);

  JPABaseEmitter *emitterSmoke = gpMarioParticleManager->emitAndBindToPosPtr(
      0x136, &player->mPosition, 1, player);
  emitterSmoke->mSize2.set(player->mSize.x * fireScale, player->mSize.y * fireScale,
                          player->mSize.z * fireScale);

  JPABaseEmitter *emitterEmber = gpMarioParticleManager->emitAndBindToPosPtr(
      0x137, &player->mPosition, 1, player);
  emitterEmber->mSize2.set(player->mSize.x * fireScale, player->mSize.y * fireScale,
                          player->mSize.z * fireScale);


  const s32 fireFrame = params->mFireTimer % MaxFireDamageTime;
  params->mFireTimer += 1;

  if (player->mFludd->mCurrentNozzle == TWaterGun::Hover &&
      player->mFludd->mIsEmitWater) 
    params->mFireTimerMax -= 1;

  if (fireFrame == 0) {
    decHP__6TMarioFi(player, 1);
    changePlayerStatus__6TMarioFUlUlb(player, 0x208B6, 0, 0);
    startVoice__6TMarioFUl(player, 0x7849);
  }

  if (!(player->mState & static_cast<u32>(TMario::STATE_AIRBORN))) {
    switch (static_cast<TMario::State>(player->mState)) {
    case TMario::STATE_TURNING_MID:
      break;
    case TMario::STATE_IDLE:
      changePlayerStatus__6TMarioFUlUlb(player, TMario::STATE_RUNNING, 0, false);
    default:
      setPlayerVelocity__6TMarioFf(player, 50.0f * player->mSize.z);
      player->mActionState = 1;
    }
  }

  if (params->mFireTimer > params->mFireTimerMax)
    Util::Mario::extinguishPlayer(player, true);
}

static void flameMario(TEffectObjBase *fire, u32 message) {
  s32 marioIdx;
  SME_FROM_GPR(30, marioIdx);

  TMario *player = reinterpret_cast<TMario *>(fire->mCollidingObjs[marioIdx]);
  Util::Mario::setFireToPlayer(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x80038148, 0x80038200, 0, 0), flameMario);