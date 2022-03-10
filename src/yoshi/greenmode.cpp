#include "SME.hxx"

using namespace SME;

#if SME_GREEN_YOSHI

SME_WRITE_32(SME_PORT_REGION(0x8026E068, 0x80265DF4, 0, 0),
             0x2C000001); // Turn green when out of juice
SME_WRITE_32(SME_PORT_REGION(0x8026E0A0, 0x80265E2C, 0, 0),
             0x60000000); // No flickering
SME_WRITE_32(SME_PORT_REGION(0x8026EE14, 0x80266BA0, 0, 0),
             0x4800020C); // Yoshi can't starve

// 0x8026EB00, 0x8026EBFC, 0x8026F218
// extern -> SME.cpp
static bool isYoshiDie(TMario *player) {
  return !Util::Yoshi::isGreenYoshi(player);
}
SME_PATCH_BL(SME_PORT_REGION(0x8026EB00, 0x80266BDC, 0, 0), isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026EB04, 0x80266BE0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026EB08, 0x80266BE4, 0, 0), 0x41820518);

SME_PATCH_BL(SME_PORT_REGION(0x8026EBFC, 0x80266988, 0, 0), isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026EC00, 0x8026698C, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026EC04, 0x80266990, 0, 0), 0x4182041C);

SME_PATCH_BL(SME_PORT_REGION(0x8026F218, 0x80266FA4, 0, 0), isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026F21C, 0x80266FA8, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026F220, 0x80266FAC, 0, 0), 0x41820164);

// 0x8024F240
// extern -> SME.cpp
static void maybeYoshiDrown(TYoshi *yoshi) {
  if (!Util::Yoshi::isGreenYoshi(yoshi))
    disappear__6TYoshiFv(yoshi);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024F240, 0x80246FCC, 0, 0), maybeYoshiDrown);

// 0x802810F8
// extern -> SME.cpp
static bool canMountYoshi() {
  TMario *player;
  SME_FROM_GPR(31, player);

  const Class::TPlayerParams *params =
      TGlobals::getPlayerData(player)->getParams();

  if (params->mSizeMultiplier.get() *
          Class::TStageParams::sStageConfig->mPlayerSizeMultiplier.get() >
      1.5f)
    return false;

  if (player->mState & static_cast<u32>(TMario::State::WATERBORN))
    return params->mCanRideYoshi.get();
  else
    return ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) &&
            params->mCanRideYoshi.get());
}
SME_PATCH_BL(SME_PORT_REGION(0x802810F8, 0x80278E84, 0, 0), canMountYoshi);
SME_WRITE_32(SME_PORT_REGION(0x802810FC, 0x80278E88, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x80281100, 0x80278E8C, 0, 0), 0x807F03F0);
SME_WRITE_32(SME_PORT_REGION(0x80281104, 0x80278E90, 0, 0), 0x38830020);
SME_WRITE_32(SME_PORT_REGION(0x80281110, 0x80278E9C, 0, 0), 0x60000000);

static f32 getYoshiYPos(TYoshi *yoshi) {
  TMario *player;
  SME_FROM_GPR(31, player);

  return player->mYoshi->mCoordinates.y;
}
SME_PATCH_BL(SME_PORT_REGION(0x80281148, 0x80278ED4, 0, 0), getYoshiYPos);

static void canYoshiSpray(TWaterGun *gpWaterGun) {
  TMario *player = gpWaterGun->mMario;
  if (!player->mYoshi)
    return;

  if (!Util::Yoshi::isGreenYoshiMounted(player->mYoshi))
    emit__9TWaterGunFv(gpWaterGun);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E58C, 0x80246318, 0, 0), canYoshiSpray);

static u32 calcYoshiSwimVelocity(TMario *player, u32 arg1) {
  Class::TPlayerData *playerParams = SME::TGlobals::getPlayerData(player);
  if (!playerParams) {
    return jumpProcess__6TMarioFi(player, arg1);
  }

  if (Class::TStageParams::sStageConfig->mIsYoshiHungry.get())
    return jumpProcess__6TMarioFi(player, arg1);

  if (!player->mYoshi)
    return jumpProcess__6TMarioFi(player, arg1);

  if (!Util::Yoshi::isGreenYoshiMounted(player->mYoshi))
    return jumpProcess__6TMarioFi(player, arg1);

  if (player->mController->mButtons.mInput & TMarioGamePad::EButtons::A) {
    if (playerParams->mYoshiWaterSpeed.y > 12.0f)
      playerParams->mYoshiWaterSpeed.y = 12.0f;
    else
      playerParams->mYoshiWaterSpeed.y += 0.34375f;
  } else {
    if (playerParams->mYoshiWaterSpeed.y < -12.0f)
      playerParams->mYoshiWaterSpeed.y = -12.0f;
    else
      playerParams->mYoshiWaterSpeed.y -= 0.34375f;
  }
  player->mSpeed.y = playerParams->mYoshiWaterSpeed.y;
  return jumpProcess__6TMarioFi(player, arg1);
}
SME_PATCH_BL(SME_PORT_REGION(0x80273198, 0x8026AF24, 0, 0), calcYoshiSwimVelocity);

static u32 isYoshiWaterFlutter() {
  TYoshi *yoshi;
  u32 animID;
  SME_FROM_GPR(29, yoshi);
  SME_FROM_GPR(30, animID);

  if (!Class::TStageParams::sStageConfig->mIsYoshiHungry.get() &&
      Util::Yoshi::isGreenYoshiAscendingWater(yoshi->mMario))
    animID = 9;

  if ((animID & 0xFFFF) == 24)
    animID = 15;

  return animID;
}
SME_PATCH_BL(SME_PORT_REGION(0x80270078, 0x80267E04, 0, 0), isYoshiWaterFlutter);
SME_WRITE_32(SME_PORT_REGION(0x8027007C, 0x80267E08, 0, 0), 0x7C7E1B78);
SME_WRITE_32(SME_PORT_REGION(0x80270080, 0x80267E0C, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80270084, 0x80267E10, 0, 0), 0x60000000);

// 0x8026FE84 NEEDS ADDI R4, R3, 0
static u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player) {
  if (!Class::TStageParams::sStageConfig->mIsYoshiHungry.get())
    return player->mState;

  if (Util::Yoshi::isGreenYoshiAscendingWater(player))
    return (player->mState & 0xFFFFFBFF) |
           static_cast<u32>(TMario::State::AIRBORN);
  else
    return player->mState;
}
// SME_PATCH_BL(SME_PORT_REGION(0x8026FE84, 0, 0, 0), isYoshiValidWaterFlutter);

static bool isYoshiValidDrip(TYoshi *yoshi) {
  return Util::Yoshi::isMounted(yoshi) && !Util::Yoshi::isGreenYoshi(yoshi);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024E788,0x80246514, 0, 0), isYoshiValidDrip);

#endif