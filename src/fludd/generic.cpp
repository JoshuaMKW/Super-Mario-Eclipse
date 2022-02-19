#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

using namespace SME;

static bool isPumpOK(TMarioAnimeData *animeData) {
  return (animeData->mFluddEnabled != TMarioAnimeData::FLUDD::FLUDD_DISABLED &&
          TGlobals::getPlayerData(gpMarioAddress)->mCurJump <= 1);
}
SME_PATCH_B(SME_PORT_REGION(0x80248F14, 0, 0, 0), isPumpOK);
SME_WRITE_32(SME_PORT_REGION(0x803DCA00, 0, 0, 0), // Allow dive spray
             0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);

static TWaterGun *bindFluddtojoint() {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (!SME::TGlobals::getPlayerData(player))
    return player->mFludd;

  player->mBindBoneIDArray[0] =
      SME::TGlobals::getPlayerData(player)->getNozzleBoneID(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mCurrentNozzle));

  return player->mFludd;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0, 0, 0), bindFluddtojoint);

static bool hasWaterCardOpen() {
  TGCConsole2 *gcConsole;
  SME_FROM_GPR(31, gcConsole);

  const Class::TPlayerData *playerParams =
      TGlobals::getPlayerData(gpMarioAddress);

  if (gpMarioAddress->mYoshi->mState != TYoshi::State::MOUNTED &&
      !gpMarioAddress->mAttributes.mHasFludd && !gcConsole->mWaterCardFalling &&
      gcConsole->mIsWaterCard)
    startDisappearTank__11TGCConsole2Fv(gcConsole);
  else if (gpMarioAddress->mYoshi->mState == TYoshi::State::MOUNTED)
    gpMarioAddress->mAttributes.mHasFludd = true;

  return gcConsole->mIsWaterCard;
}
SME_PATCH_BL(SME_PORT_REGION(0x8014206C, 0, 0, 0), hasWaterCardOpen);
SME_WRITE_32(SME_PORT_REGION(0x80142070, 0, 0, 0), 0x28030000);

static bool canCollectFluddItem(TMario *player) {
  return onYoshi__6TMarioCFv(player) ||
         !TGlobals::getPlayerData(player)->getCanUseFludd();
}
SME_PATCH_BL(SME_PORT_REGION(0x80283058, 0, 0, 0), canCollectFluddItem);

static s32 sNozzleBuzzCounter = -1;
static bool canCollectFluddItem_() {
  TMario *player;
  SME_FROM_GPR(30, player);

  const bool isOnYoshi = onYoshi__6TMarioCFv(player);
  if (!TGlobals::getPlayerData(player)->getCanUseFludd()) {
    if (gpMSound->gateCheck(0x483E) && sNozzleBuzzCounter < 0) {
      MSoundSESystem::MSoundSE::startSoundSystemSE(0x483E, 0, nullptr, 0);
      sNozzleBuzzCounter = 120;
    } else {
      sNozzleBuzzCounter -= 1;
    }
  }
  return isOnYoshi || !TGlobals::getPlayerData(player)->getCanUseFludd();
}
SME_PATCH_BL(SME_PORT_REGION(0x801BBD48, 0, 0, 0), canCollectFluddItem);

static void resetNozzleBuzzer(TMapObjGeneral *obj) {
  if (obj->mNumObjs <= 0) {
    sNozzleBuzzCounter = Max(sNozzleBuzzCounter - 1, -1);
  }
  control__14TMapObjGeneralFv(obj);
}
SME_PATCH_BL(SME_PORT_REGION(0x801BBBF8, 0, 0, 0), resetNozzleBuzzer);

static void checkRocketNozzleDiveBlast(TNozzleTrigger *nozzle, u32 r4,
                                       TWaterEmitInfo *emitInfo) {
  TMario *player = nozzle->mFludd->mMario;

  if (nozzle->mFludd->mCurrentNozzle != TWaterGun::Rocket)
    return;

  nozzle->mForwardSpeedFactor =
      player->mState != static_cast<u32>(TMario::State::DIVE) ? 0.0f : 1.0f;
}

// Fludd mods hook
void fluddEmitModWrapper(TNozzleTrigger *nozzle, u32 r4,
                         TWaterEmitInfo *emitInfo) {
  void (*virtualFunc)(TNozzleTrigger *, u32, TWaterEmitInfo *);
  SME_FROM_GPR(12, virtualFunc);

  /* CODE HERE */

  extern void checkSpamHover(TNozzleTrigger * nozzle, u32 r4,
                             TWaterEmitInfo * emitInfo);
  checkSpamHover(nozzle, r4, emitInfo);

  checkRocketNozzleDiveBlast(nozzle, r4, emitInfo);

  virtualFunc(nozzle, r4, emitInfo);
}
SME_PATCH_BL(SME_PORT_REGION(0x8026C018, 0, 0, 0), fluddEmitModWrapper);