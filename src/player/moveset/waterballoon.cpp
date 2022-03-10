#include "obj/WaterBalloon.hxx"
#include "Player.hxx"
#include "libs/sContainer.hxx"


#include "SME.hxx"
#include "sms/actor/Mario.hxx"


using namespace SME;

#if SME_WATERBALLOONS
static TRingBuffer<TWaterBalloon> *sWaterBalloons[4] = {nullptr, nullptr,
                                                        nullptr, nullptr};

void initWaterBalloons() {
  TRingBuffer<TWaterBalloon> *balloons =
      new TRingBuffer<TWaterBalloon>(16, false);
  sWaterBalloons[0] = balloons;
  for (int i = 0; i < balloons->capacity(); ++i) {
    TWaterBalloon *balloon = new TWaterBalloon("waterballoon");
    balloon->initAndRegister("waterballoon");
    balloons->push(balloon);
  }
}

static void createWaterBalloonAndThrow(TMario *player) {
  gunExec__6TMarioFv(player);

  if (player->mHeldObject || player->mAttributes.mHasFludd)
    return;

  if (!(player->mController->mButtons.mFrameInput & TMarioGamePad::R))
    return;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  TRingBuffer<TWaterBalloon> &balloons = *sWaterBalloons[0];

  TWaterBalloon *waterBalloon = sWaterBalloons[0]->next();
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
SME_PATCH_BL(SME_PORT_REGION(0x8024E2A0, 0x8024602C, 0, 0), createWaterBalloonAndThrow);
#else

void initWaterBalloons() {}

#endif
