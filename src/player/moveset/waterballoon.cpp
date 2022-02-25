#include "obj/WaterBalloon.hxx"
#include "libs/sContainer.hxx"
#include "Player.hxx"

#include "sms/actor/Mario.hxx"
#include "SME.hxx"

using namespace SME;

static void createWaterBalloonAndThrow(TMario *player) {
  gunExec__6TMarioFv(player);

  if (player->mHeldObject || player->mAttributes.mHasFludd)
    return;

  if (!(player->mController->mButtons.mFrameInput & TMarioGamePad::R))
    return;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TRingBuffer<TWaterBalloon> &balloons = playerData->mBalloons;

  TWaterBalloon *waterBalloon = reinterpret_cast<TWaterBalloon *>(balloons.next());
  waterBalloon->mPosition.set(player->mPosition);
  waterBalloon->appear();

  // Have player grab balloon
  if (waterBalloon->receiveMessage(player, 4)) { // 4 = grab
    player->mHeldObject = waterBalloon;
    player->_380 = 2;
  } else {
    player->mHeldObject = nullptr;
  }

  if (player->mState == static_cast<u32>(TMario::State::IDLE)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 0); // 0x80000387
  } else if (player->mState == static_cast<u32>(TMario::State::RUNNING)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x80000588, 0, 0);
  } else if (player->mState == static_cast<u32>(TMario::State::JUMP) ||
             player->mState == static_cast<u32>(TMario::State::D_JUMP) ||
             player->mState == static_cast<u32>(TMario::State::FALL)) {
    changePlayerStatus__6TMarioFUlUlb(player, 0x820008AB, 0, 0);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E2A0, 0, 0, 0), createWaterBalloonAndThrow);