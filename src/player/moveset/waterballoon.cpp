#include "obj/WaterBalloon.hxx"
#include "Player.hxx"
#include "libs/sContainer.hxx"

#include "SME.hxx"
#include "sms/actor/Mario.hxx"

using namespace SME;

#if SME_WATERBALLOONS

static void createWaterBalloonAndThrow(TMario *player) {
  gunExec__6TMarioFv(player);

  if (gpMarDirector->mCurState != 4)
    return;

  if (player->mState == 0x200009 || player->mState == static_cast<u32>(TMario::State::SHINE_C))
    return;

  if (player->mHeldObject || player->mAttributes.mHasFludd)
    return;

  if (!(player->mController->mButtons.mFrameInput & TMarioGamePad::R))
    return;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TRingBuffer<TWaterBalloon> &balloons = playerData->mBalloons;

  TWaterBalloon *waterBalloon = balloons.next();
  waterBalloon->mPosition.set(player->mPosition);
  waterBalloon->appear();

  // Have player grab balloon
  if (waterBalloon->receiveMessage(player, 4)) { // 4 = grab
    player->mHeldObject = waterBalloon;
    player->_380 = 2;
  } else {
    player->mHeldObject = nullptr;
    return;
  }

  //   if (player->mState == static_cast<u32>(TMario::State::IDLE)) {
  //     changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 1); //
  //     0x80000387
  //   } else if (player->mState == static_cast<u32>(TMario::State::RUNNING)) {
  //     changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 1);
  //   } else if (player->mState == static_cast<u32>(TMario::State::JUMP) ||
  //              player->mState == static_cast<u32>(TMario::State::D_JUMP) ||
  //              player->mState == static_cast<u32>(TMario::State::FALL)) {
  //     changePlayerStatus__6TMarioFUlUlb(player, 0x820008AB, 0, 1);
  //   }
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E2A0, 0, 0, 0), createWaterBalloonAndThrow);

static void initConductor(TConductor *conductor) {
  conductor->init();
  for (int i = 0; i < TGlobals::getMaxPlayers(); ++i) {
    Class::TPlayerData *pd = TGlobals::getPlayerData(i);
    if (!pd)
      continue;

    for (int i = 0; i < pd->mBalloons.capacity(); ++i) {
      TWaterBalloon *balloon = new TWaterBalloon("waterballoon");
      // gpConductor->registerAloneActor(balloon);
      balloon->initAndRegister("waterballoon");
      balloon->mSprayPushSpeed = 0.1f;
      balloon->mAdditionalHeldHeight = 24.0f;
      pd->mBalloons.push(balloon);
    }
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x802B8E0C, 0, 0, 0), initConductor);

#else

void initWaterBalloons() {}

#endif