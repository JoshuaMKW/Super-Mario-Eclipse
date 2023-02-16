#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "p_common.hxx"

using namespace BetterSMS;

SMS_WRITE_32(SMS_PORT_REGION(0x8026E068, 0x80265DF4, 0, 0),
             0x2C000001); // Turn green when out of juice
SMS_WRITE_32(SMS_PORT_REGION(0x8026E0A0, 0x80265E2C, 0, 0),
             0x60000000); // No flickering
SMS_WRITE_32(SMS_PORT_REGION(0x8026EE14, 0x80266BA0, 0, 0),
             0x4800020C); // Yoshi can't starve

// 0x8026EB00, 0x8026EBFC, 0x8026F218
// extern -> SME.cpp
static bool isYoshiDie(TMario *player) {
  return !Yoshi::isGreenYoshi(player);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026EB00, 0x80266BDC, 0, 0), isYoshiDie);
SMS_WRITE_32(SMS_PORT_REGION(0x8026EB04, 0x80266BE0, 0, 0), 0x2C030000);
SMS_WRITE_32(SMS_PORT_REGION(0x8026EB08, 0x80266BE4, 0, 0), 0x41820518);

SMS_PATCH_BL(SMS_PORT_REGION(0x8026EBFC, 0x80266988, 0, 0), isYoshiDie);
SMS_WRITE_32(SMS_PORT_REGION(0x8026EC00, 0x8026698C, 0, 0), 0x2C030000);
SMS_WRITE_32(SMS_PORT_REGION(0x8026EC04, 0x80266990, 0, 0), 0x4182041C);

SMS_PATCH_BL(SMS_PORT_REGION(0x8026F218, 0x80266FA4, 0, 0), isYoshiDie);
SMS_WRITE_32(SMS_PORT_REGION(0x8026F21C, 0x80266FA8, 0, 0), 0x2C030000);
SMS_WRITE_32(SMS_PORT_REGION(0x8026F220, 0x80266FAC, 0, 0), 0x41820164);

// 0x8024F240
// extern -> SME.cpp
static void maybeYoshiDrown(TYoshi *yoshi) {
  if (!Yoshi::isGreenYoshi(yoshi))
    disappear__6TYoshiFv(yoshi);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024F240, 0x80246FCC, 0, 0), maybeYoshiDrown);

// 0x802810F8
// extern -> SME.cpp
static bool canMountYoshi() {
  TMario *player;
  SMS_FROM_GPR(31, player);

  auto *stage_config = Stage::getStageConfiguration();

  auto *params = Player::getData(player)->getParams();

  // Prevent very large players from mounting
  if (params->mScaleMultiplier.get() * stage_config->mPlayerSizeMultiplier.get() > 1.5f)
      return false;

  if (player->mState & static_cast<u32>(TMario::STATE_WATERBORN))
    return params->mCanRideYoshi.get();
  else
    return ((player->mState & static_cast<u32>(TMario::STATE_AIRBORN)) &&
            params->mCanRideYoshi.get());
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802810F8, 0x80278E84, 0, 0), canMountYoshi);
SMS_WRITE_32(SMS_PORT_REGION(0x802810FC, 0x80278E88, 0, 0), 0x2C030000);
SMS_WRITE_32(SMS_PORT_REGION(0x80281100, 0x80278E8C, 0, 0), 0x807F03F0);
SMS_WRITE_32(SMS_PORT_REGION(0x80281104, 0x80278E90, 0, 0), 0x38830020);
SMS_WRITE_32(SMS_PORT_REGION(0x80281110, 0x80278E9C, 0, 0), 0x60000000);

static f32 getYoshiYPos(TYoshi *yoshi) {
  TMario *player;
  SMS_FROM_GPR(31, player);

  return player->mYoshi->mTranslation.y;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80281148, 0x80278ED4, 0, 0), getYoshiYPos);

static void canYoshiSpray(TWaterGun *fludd) {
  TMario *player = fludd->mMario;
  if (!player->mYoshi)
    return;

  if (!Yoshi::isGreenYoshiMounted(player->mYoshi))
    fludd->emit();
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024E58C, 0x80246318, 0, 0), canYoshiSpray);

static u32 calcYoshiSwimVelocity(TMario *player, u32 arg1) {
    auto *player_data = Player::getData(player);
    if (!player_data) {
        return player->jumpProcess(arg1);
    }

    if (!player->mYoshi)
        return player->jumpProcess(arg1);

    if (!Yoshi::isGreenYoshiMounted(player->mYoshi))
        return player->jumpProcess(arg1);

    if (player->mController->mButtons.mInput & TMarioGamePad::EButtons::A) {
        if (player_data->mYoshiWaterSpeed.y > 12.0f)
            player_data->mYoshiWaterSpeed.y = 12.0f;
        else
            player_data->mYoshiWaterSpeed.y += 0.34375f;
    } else {
        if (player_data->mYoshiWaterSpeed.y < -12.0f)
            player_data->mYoshiWaterSpeed.y = -12.0f;
        else
            player_data->mYoshiWaterSpeed.y -= 0.34375f;
    }

    player->mSpeed.y = player_data->mYoshiWaterSpeed.y;
    return player->jumpProcess(arg1);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80273198, 0x8026AF24, 0, 0), calcYoshiSwimVelocity);

static u32 isYoshiWaterFlutter() {
  TYoshi *yoshi;
  u32 animID;
  SMS_FROM_GPR(29, yoshi);
  SMS_FROM_GPR(30, animID);

  if (Yoshi::isGreenYoshiAscendingWater(yoshi->mMario))
    animID = 9;

  if ((animID & 0xFFFF) == 24)
    animID = 15;

  return animID;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80270078, 0x80267E04, 0, 0), isYoshiWaterFlutter);
SMS_WRITE_32(SMS_PORT_REGION(0x8027007C, 0x80267E08, 0, 0), 0x7C7E1B78);
SMS_WRITE_32(SMS_PORT_REGION(0x80270080, 0x80267E0C, 0, 0), 0x60000000);
SMS_WRITE_32(SMS_PORT_REGION(0x80270084, 0x80267E10, 0, 0), 0x60000000);

// 0x8026FE84 NEEDS ADDI R4, R3, 0
static u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player) {
  if (Yoshi::isGreenYoshiAscendingWater(player))
    return (player->mState & 0xFFFFFBFF) |
           static_cast<u32>(TMario::STATE_AIRBORN);
  else
    return player->mState;
}
// SMS_PATCH_BL(SMS_PORT_REGION(0x8026FE84, 0, 0, 0), isYoshiValidWaterFlutter);

static bool isYoshiValidDrip(TYoshi *yoshi) {
  return Yoshi::isMounted(yoshi) && !Yoshi::isGreenYoshi(yoshi);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024E788, 0x80246514, 0, 0), isYoshiValidDrip);

static void fixYoshiJuiceDecrement() {
    TYoshi* yoshi;
    SMS_FROM_GPR(30, yoshi);

    TMario* player = yoshi->mMario;
    if (player->mFludd->mIsEmitWater && yoshi->mState == TYoshi::MOUNTED)
        yoshi->mCurJuice -= player->mFludd->mEmitInfo->mNum.get();
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026E810, 0x8026659C, 0, 0), fixYoshiJuiceDecrement);

BETTER_SMS_FOR_CALLBACK void adjustYoshiTounge(TMario *player, bool isMario) {
    if (!player->mYoshi)
        return;

    if (Yoshi::isGreenYoshi(player->mYoshi)) {
        *(f32 *)SMS_PORT_REGION(0x80415F4C, 0x8040D4A4, 0, 0) = 480.0f;  // tounge
        *(f32 *)SMS_PORT_REGION(0x80415F68, 0x8040D4A8, 0, 0) = 16384.0f;
    } else {
        *(f32 *)SMS_PORT_REGION(0x80415F4C, 0x8040D4A4, 0, 0) = 300.0f;
        *(f32 *)SMS_PORT_REGION(0x80415F68, 0x8040D4A8, 0, 0) = 10000.0f;
    }
}