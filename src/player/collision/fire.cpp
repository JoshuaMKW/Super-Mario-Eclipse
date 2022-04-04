#include "sms/SMS.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/manager/EffectObjManager.hxx"
#include "sms/object/EffectObjBase.hxx"

#include "Globals.hxx"
#include "params/MarioParams.hxx"

using namespace SME;
using namespace SME::Class;

static TEffectObjBase *sEffectObjs[SME_MAX_PLAYERS];

namespace SME::Util::Mario {

void setFireToPlayer(TMario *player) {
  TPlayerData *params = TGlobals::getPlayerData(player);
  params->mIsOnFire = true;
  params->mFireTimer = 0;
}

void extinguishPlayer(TMario *player) {
  TPlayerData *params = TGlobals::getPlayerData(player);
  params->mIsOnFire = false;
  params->mFireTimer = 0;
}

} // namespace SME::Util::Mario


static JPABaseEmitter *sFireEmitters[SME_MAX_PLAYERS];
static JPABaseEmitter *sSmokeEmitters[SME_MAX_PLAYERS];

static constexpr s32 MaxFireDamageTime = 250;

void blazePlayer(TMario *player) {
  TPlayerData *params = TGlobals::getPlayerData(player);

  JPABaseEmitter *emitterFire = gpMarioParticleManager->emitAndBindToPosPtr(
      0x135, &player->mPosition, 1, player);
  emitterFire->mSize.set(player->mSize.x * 2.0f, player->mSize.y * 2.0f,
                         player->mSize.z * 2.0f);
  sFireEmitters[TGlobals::getIDFromPlayer(player)] = emitterFire;

  JPABaseEmitter *emitterSmoke = gpMarioParticleManager->emitAndBindToPosPtr(
      0x136, &player->mPosition, 1, player);
  emitterSmoke->mSize.set(player->mSize.x * 2.0f, player->mSize.y * 2.0f,
                         player->mSize.z * 2.0f);
  sSmokeEmitters[TGlobals::getIDFromPlayer(player)] = emitterSmoke;

  params->mFireTimer += 1;
  if (params->mFireTimer > MaxFireDamageTime) {
      decHP__6TMarioFi(player, 1);
      startVoice__6TMarioFUl(player, 0x7849);
      params->mFireTimer = 0;
  }
}