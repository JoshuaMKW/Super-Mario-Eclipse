#include "SME.hxx"

using namespace SME;

bool TYoshi::isMounted() { return this->mState == TYoshi::MOUNTED; }

bool TYoshi::isMounted(TMario *player) {
  return player->mYoshi->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshi() { return this->mType == TYoshi::GREEN; }

bool TYoshi::isGreenYoshi(TMario *player) {
  return player->mYoshi->isGreenYoshi();
}

bool TYoshi::isGreenYoshiMounted() {
  return this->isGreenYoshi() && this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshiMounted(TMario *player) {
  return player->mYoshi->isGreenYoshiMounted();
}

bool TYoshi::isGreenYoshiAscendingWater(TMario *player) {
  return player->mAttributes.mIsWater &&
         player->mController->mButtons.mInput & TMarioGamePad::Buttons::A &&
         player->mYoshi->isGreenYoshiMounted();
}

//--------Water--Volatility--------//

// 0x8026EB00, 0x8026EBFC, 0x8026F218
// extern -> SME.cpp
bool Patch::Yoshi::isYoshiDie(TMario *player) {
  return !player->mYoshi->isGreenYoshi();
}

// 0x801BC868
// extern -> SME.cpp
bool Patch::Yoshi::isYoshiEggNeedFruit(THitActor *gpFruit) {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi ||
      !SME::Class::TSMEFile::sStageConfig.Yoshi.mIsEggFree)
    return isFruit__11TMapObjBaseFP9THitActor(gpFruit);

  return true;
}

// 0x801BC8B4
// extern -> SME.cpp
u8 Patch::Yoshi::isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit) {
  const SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(gpMarioAddress);
  if (gpEgg->mState == 14 || gpEgg->mState == 6)
    return 0;
  else if (!playerParams->getParams()->mCanRideYoshi.get())
    return 0;
  else if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
               SME::Class::TSMEFile::MAGIC ||
           !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi ||
           !SME::Class::TSMEFile::sStageConfig.Yoshi.mIsEggFree) {
    if (gpEgg->mWantedFruit != gpFruit->mObjectID)
      return 2;
    else
      return 1;
  } else
    return 1;
}

// 0x8024D68C
// extern -> SME.cpp
bool Patch::Yoshi::isYoshiMaintainFluddModel() {
  TMario *player;
  SME_FROM_GPR(31, player);

  const SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(player);

  if (player->mYoshi->mState == TYoshi::MOUNTED)
    return (playerParams->mFluddHistory.mHadFludd &
            player->mAttributes.mHasFludd);
  else
    return player->mAttributes.mHasFludd;
}

// 0x8024F240
// extern -> SME.cpp
void Patch::Yoshi::maybeYoshiDrown(TYoshi *yoshi) {
  if (!yoshi->isGreenYoshi())
    disappear__6TYoshiFv(yoshi);
}

// 0x802810F8
// extern -> SME.cpp
bool Patch::Yoshi::canMountYoshi(u32 state) {
  TMario *player;
  SME_FROM_GPR(31, player);

  const SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(player);

  if (state & static_cast<u32>(TMario::State::WATERBORN))
    return playerParams->getParams()->mCanRideYoshi.get();
  else
    return ((state & static_cast<u32>(TMario::State::AIRBORN)) &&
            playerParams->getParams()->mCanRideYoshi.get());
}

// 0x80281148
// extern -> SME.cpp
f32 Patch::Yoshi::getYoshiYPos_(TYoshi *yoshi) {
  TMario *player;
  SME_FROM_GPR(31, player);

  return player->mYoshi->mCoordinates.y;
}

// 0x8026E810
// extern -> SME.cpp
void Patch::Yoshi::fixYoshiJuiceDecrement() {
  TYoshi *yoshi;
  SME_FROM_GPR(30, yoshi);

  TMario *player = yoshi->mMario;
  if (player->mFludd->mIsEmitWater && yoshi->isMounted())
    yoshi->mCurJuice -= player->mFludd->mEmitInfo->mEmitCount;
}

// 0x8024E58C
// extern -> SME.cpp
void Patch::Yoshi::canYoshiSpray(TWaterGun *gpWaterGun) {
  TMario *player = gpWaterGun->mMario;
  if (!player->mYoshi)
    return;

  if (!player->mYoshi->isGreenYoshiMounted())
    emit__9TWaterGunFv(gpWaterGun);
}

// 0x80273198
// extern -> SME.cpp
u32 Patch::Yoshi::calcYoshiSwimVelocity(TMario *player, u32 arg1) {
  SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(player);

  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi)
    return jumpProcess__6TMarioFi(player, arg1);
  else if (SME::Class::TSMEFile::sStageConfig.Yoshi.mYoshiHungry)
    return jumpProcess__6TMarioFi(player, arg1);

  if (!player->mYoshi)
    return jumpProcess__6TMarioFi(player, arg1);

  if (!player->mYoshi->isGreenYoshiMounted())
    return jumpProcess__6TMarioFi(player, arg1);

  if (player->mController->mButtons.mInput & TMarioGamePad::Buttons::A) {
    if (playerParams->mYoshiWaterSpeed.y > 12)
      playerParams->mYoshiWaterSpeed.y = 12;
    else
      playerParams->mYoshiWaterSpeed.y += 0.34375;
  } else {
    if (playerParams->mYoshiWaterSpeed.y < -12)
      playerParams->mYoshiWaterSpeed.y = -12;
    else
      playerParams->mYoshiWaterSpeed.y -= 0.34375;
  }
  player->mSpeed.y = playerParams->mYoshiWaterSpeed.y;
  return jumpProcess__6TMarioFi(player, arg1);
}

// 0x80270078
// extern -> SME.cpp
u32 Patch::Yoshi::isYoshiWaterFlutter() {
  TYoshi *yoshi;
  u32 animID;
  SME_FROM_GPR(29, yoshi);
  SME_FROM_GPR(30, animID);

  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi &&
      SME::Class::TSMEFile::sStageConfig.Yoshi.mYoshiHungry &&
      TYoshi::isGreenYoshiAscendingWater(yoshi->mMario))
    animID = 9;

  if ((animID & 0xFFFF) == 24)
    animID = 15;

  return animID;
}

// 0x8026FE84 NEEDS ADDI R4, R3, 0
u32 Patch::Yoshi::isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1,
                                           TMario *player) {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi)
    return player->mState;
  else if (SME::Class::TSMEFile::sStageConfig.Yoshi.mYoshiHungry)
    return player->mState;

  if (TYoshi::isGreenYoshiAscendingWater(player))
    return (player->mState & 0xFFFFFBFF) | static_cast<u32>(TMario::State::AIRBORN);
  else
    return player->mState;
}

// 0x8024E788
// extern -> SME.cpp
bool Patch::Yoshi::isYoshiValidDrip(TYoshi *yoshi) {
  return yoshi->isMounted() && !yoshi->isGreenYoshi();
}

// 0x801BC128
// extern -> SME.cpp
void Patch::Yoshi::initFreeEggCard(MActorAnmBck *bckData) {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi ||
      !SME::Class::TSMEFile::sStageConfig.Yoshi.mIsEggFree)
    return;

  bckData->mFrameCtrl.mFrameRate = 11;
}

// 0x801BC380
// extern -> SME.cpp
u32 Patch::Yoshi::checkFreeEggCard(MActorAnmBck *bckData) {
  if (SME::Class::TSMEFile::sStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig.GlobalFlags.mIsYoshi ||
      !SME::Class::TSMEFile::sStageConfig.Yoshi.mIsEggFree)
    return 0;

  bckData->mFrameCtrl.mFrameRate = 11;
  return 0;
}

// 0x8028121C
// extern -> SME.cpp
void Patch::Yoshi::saveNozzles(TYoshi *yoshi) {
  TMario *player = yoshi->mMario;

  SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(player);

  playerParams->mFluddHistory.mMainNozzle = player->mFludd->mCurrentNozzle;
  playerParams->mFluddHistory.mSecondNozzle = player->mFludd->mSecondNozzle;
  playerParams->mFluddHistory.mWaterLevel = player->mFludd->mCurrentWater;
  playerParams->mFluddHistory.mHadFludd = player->mAttributes.mHasFludd;
  ride__6TYoshiFv(yoshi);
}

// 0x8024EC18
// extern -> SME.cpp
void Patch::Yoshi::restoreNozzles(TMario *player) {
  SME::Class::TPlayerParams *playerParams =
      SME::TGlobals::sGlobals.getPlayerParams(player);
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