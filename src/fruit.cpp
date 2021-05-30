#include "sms/object/ResetFruit.hxx"

#include "SME.hxx"

using namespace SME;

// 0x801E542C
// extern -> SME.cpp
bool Patch::Fruit::canFruitDieWater(TResetFruit *fruit) {
  if (fruit->mObjectID == TResetFruit::DURIAN) {
    fruit->touchWaterSurface();
    return true;
  } else {
    fruit->mStateFlags.mHasPhysics = true;
    if (gpMSound->gateCheck(14453)) {
      Vec fruitPos;
      fruit->JSGGetTranslation(&fruitPos);
      fruit->emitColumnWater();
      startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(
          14453, &fruitPos, 0, 0, 0, 4);
    }
  }

  return false;
}

// 0x8023F964
// extern -> SME.cpp
f32 Patch::Fruit::chooseGrabDistancing(M3UModelMario *model) {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (player->mPrevState & static_cast<u32>(TMario::State::WATERBORN)) {
    SME_TO_GPR(3, model);
    return 0.0f;
  } else {
    SME_TO_GPR(3, model);
    return 11.0f;
  }
}

// 0x8023F9DC
// extern -> SME.cpp
bool Patch::Fruit::isGrabWaitOver(TMario *player) {
  return isLast1AnimeFrame__6TMarioFv(player) |
         (player->mPrevState & static_cast<u32>(TMario::State::WATERBORN));
}