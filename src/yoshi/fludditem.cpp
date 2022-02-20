#include "SME.hxx"

using namespace SME;

#ifdef SME_YOSHI_SAVE_NOZZLE

static bool isYoshiMaintainFluddModel() {
  TMario *player;
  SME_FROM_GPR(31, player);

  const Class::TPlayerData *playerParams = TGlobals::getPlayerData(player);

  if (!playerParams)
    return player->mAttributes.mHasFludd;

  if (player->mYoshi->mState == TYoshi::MOUNTED)
    return (playerParams->mFluddHistory.mHadFludd &
            player->mAttributes.mHasFludd);
  else
    return player->mAttributes.mHasFludd;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024D68C, 0, 0, 0), isYoshiMaintainFluddModel);
SME_WRITE_32(SME_PORT_REGION(0x8024D690, 0, 0, 0), 0x2C030000);

static void saveNozzles(TYoshi *yoshi) {
  TMario *player = yoshi->mMario;

  Class::TPlayerData *playerParams = TGlobals::getPlayerData(player);
  if (!playerParams->isMario()) {
    ride__6TYoshiFv(yoshi);
    return;
  }

  playerParams->mFluddHistory.mMainNozzle = player->mFludd->mCurrentNozzle;
  playerParams->mFluddHistory.mSecondNozzle = player->mFludd->mSecondNozzle;
  playerParams->mFluddHistory.mWaterLevel = player->mFludd->mCurrentWater;
  playerParams->mFluddHistory.mHadFludd = player->mAttributes.mHasFludd;
  ride__6TYoshiFv(yoshi);
}
SME_PATCH_BL(SME_PORT_REGION(0x8028121C, 0, 0, 0), saveNozzles);

static void restoreNozzles(TMario *player) {
  Class::TPlayerData *playerParams = TGlobals::getPlayerData(player);

  if (!playerParams->isMario())
    return;

  f32 factor =
      static_cast<f32>(playerParams->mFluddHistory.mWaterLevel) /
      static_cast<f32>(
          player->mFludd->mNozzleList[playerParams->mFluddHistory.mMainNozzle]
              ->mMaxWater);
  changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(
      player->mFludd, playerParams->mFluddHistory.mSecondNozzle, 1);
  normalizeNozzle__6TMarioFv(player);
  player->mFludd->mCurrentWater =
      player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
          ->mMaxWater *
      factor;
  player->mAttributes.mHasFludd = playerParams->mFluddHistory.mHadFludd;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024EC18, 0, 0, 0), restoreNozzles);
SME_WRITE_32(SME_PORT_REGION(0x8024EC2C, 0, 0, 0), 0x60000000);

#endif