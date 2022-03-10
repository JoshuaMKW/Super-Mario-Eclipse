#include "sms/object/ResetFruit.hxx"

#include "SME.hxx"

using namespace SME;

#if SME_UNDERWATER_FRUIT

// 0x801E542C
// extern -> SME.cpp
static bool canFruitDieWater(TResetFruit *fruit) {
  if (fruit->mObjectID == TResetFruit::DURIAN) {
    fruit->touchWaterSurface();
    return true;
  } else {
    fruit->mStateFlags.asFlags.mHasPhysics = true;
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
SME_PATCH_BL(SME_PORT_REGION(0x801E542C, 0x801DD304, 0, 0), canFruitDieWater);
SME_WRITE_32(SME_PORT_REGION(0x801E5430, 0x801DD308, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x801E5434, 0x801DD30C, 0, 0), 0x41820140);

// 0x8023F964
// extern -> SME.cpp
static f32 chooseGrabDistancing(M3UModelMario *model) {
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
SME_PATCH_BL(SME_PORT_REGION(0x8023F964, 0x802376F0, 0, 0), chooseGrabDistancing);

// 0x8023F9DC
// extern -> SME.cpp
static bool isGrabWaitOver(TMario *player) {
  return isLast1AnimeFrame__6TMarioFv(player) |
         (player->mPrevState & static_cast<u32>(TMario::State::WATERBORN));
}
SME_PATCH_BL(SME_PORT_REGION(0x8023F9DC, 0x80237768, 0, 0), isGrabWaitOver);

#endif