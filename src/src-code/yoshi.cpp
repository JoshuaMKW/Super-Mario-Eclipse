#include "SME.hxx"

bool TYoshi::isMounted()
{
    return this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isMounted(TMario *player)
{
    return player->mYoshi->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshi()
{
    return this->mType == TYoshi::GREEN;
}

bool TYoshi::isGreenYoshi(TMario *player)
{
    return player->mYoshi->isGreenYoshi();
}

bool TYoshi::isGreenYoshiMounted()
{
    return this->isGreenYoshi() && this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshiMounted(TMario *player)
{
    return player->mYoshi->isGreenYoshiMounted();
}

bool TYoshi::isGreenYoshiAscendingWater(TMario *player)
{
    return player->mAttributes.mIsWater &&
           player->mController->mButtons.mInput & TMarioGamePad::Buttons::A &&
           player->mYoshi->isGreenYoshiMounted();
}

//--------Water--Volatility--------//

// 0x8026EB00, 0x8026EBFC, 0x8026F218
// extern -> SME.cpp
bool isYoshiDie(TMario *player) { return !player->mYoshi->isGreenYoshi(); }

// 0x801BC868
// extern -> SME.cpp
bool isYoshiEggNeedFruit(THitActor *gpFruit)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC ||
        !SMEFile::mStageConfig.GlobalFlags.mIsYoshi ||
        !SMEFile::mStageConfig.Yoshi.mIsEggFree)
        return isFruit__11TMapObjBaseFP9THitActor(gpFruit);

    return true;
}

// 0x801BC8B4
// extern -> SME.cpp
u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit)
{
    if (gpEgg->mState == 14 || gpEgg->mState == 6)
        return 0;
    else if (gpMarioAddress->mCustomInfo->mParams &&
             !gpMarioAddress->mCustomInfo->mParams->Attributes.mCanRideYoshi)
        return 0;
    else if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC ||
             !SMEFile::mStageConfig.GlobalFlags.mIsYoshi ||
             !SMEFile::mStageConfig.Yoshi.mIsEggFree)
    {
        if (gpEgg->mWantedFruit != gpFruit->mObjectID)
            return 2;
        else
            return 1;
    }
    else
        return 1;
}

// 0x8024D68C
// extern -> SME.cpp
bool isYoshiMaintainFluddModel()
{
    TMario *player;
    SME_FROM_GPR(r31, player);

    if (player->mYoshi->mState == TYoshi::MOUNTED)
        return (player->mCustomInfo->FluddHistory.mHadFludd & player->mAttributes.mHasFludd);
    else
        return player->mAttributes.mHasFludd;
}

// 0x8024F240
// extern -> SME.cpp
void maybeYoshiDrown(TYoshi *yoshi)
{
    if (!yoshi->isGreenYoshi())
        disappear__6TYoshiFv(yoshi);
}

// 0x802810F8
// extern -> SME.cpp
bool canMountYoshi(u32 state)
{
    TMario *player;
    SME_FROM_GPR(r31, player);

    if (player->mCustomInfo->mParams)
    {
        if (state & TMario::State::WATERBORN)
            return player->mCustomInfo->mParams->Attributes.mCanRideYoshi;
        else
            return ((state & TMario::State::AIRBORN) && player->mCustomInfo->mParams->Attributes.mCanRideYoshi);
    }
    else
        return state & TMario::State::AIRBORN;
}

// 0x80281148
// extern -> SME.cpp
f32 getYoshiYPos_(TYoshi *yoshi)
{
    TMario *player;
    SME_FROM_GPR(r31, player);

    return player->mYoshi->mCoordinates.y;
}

// 0x8026E810
// extern -> SME.cpp
void fixYoshiJuiceDecrement()
{
    TYoshi *yoshi;
    SME_FROM_GPR(r30, yoshi);

    TMario *player = yoshi->mMario;
    if (player->mFludd->mIsEmitWater && yoshi->isMounted())
        yoshi->mCurJuice -= player->mFludd->mEmitInfo->mEmitCount;
}

// 0x8024E58C
// extern -> SME.cpp
void canYoshiSpray(TWaterGun *gpWaterGun)
{
    TMario *player = gpWaterGun->mMario;
    if (!player->mYoshi)
        return;

    if (!player->mYoshi->isGreenYoshiMounted())
        emit__9TWaterGunFv(gpWaterGun);
}

// 0x80273198
// extern -> SME.cpp
u32 calcYoshiSwimVelocity(TMario *player, u32 arg1)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC ||
        !SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
        return jumpProcess__6TMarioFi(player, arg1);
    else if (SMEFile::mStageConfig.Yoshi.mYoshiHungry)
        return jumpProcess__6TMarioFi(player, arg1);

    if (!player->mYoshi)
        return jumpProcess__6TMarioFi(player, arg1);

    if (!player->mYoshi->isGreenYoshiMounted())
        return jumpProcess__6TMarioFi(player, arg1);

    if (player->mController->mButtons.mInput & TMarioGamePad::Buttons::A)
    {
        if (player->mCustomInfo->mYoshiWaterSpeed.y > 12)
            player->mCustomInfo->mYoshiWaterSpeed.y = 12;
        else
            player->mCustomInfo->mYoshiWaterSpeed.y += 0.34375;
    }
    else
    {
        if (player->mCustomInfo->mYoshiWaterSpeed.y < -12)
            player->mCustomInfo->mYoshiWaterSpeed.y = -12;
        else
            player->mCustomInfo->mYoshiWaterSpeed.y -= 0.34375;
    }
    player->mSpeed.y = player->mCustomInfo->mYoshiWaterSpeed.y;
    return jumpProcess__6TMarioFi(player, arg1);
}

// 0x80270078
// extern -> SME.cpp
u32 isYoshiWaterFlutter()
{
    TYoshi *yoshi;
    u32 animID;
    SME_FROM_GPR(r29, yoshi);
    SME_FROM_GPR(r30, animID);

    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC &&
        SMEFile::mStageConfig.GlobalFlags.mIsYoshi &&
        SMEFile::mStageConfig.Yoshi.mYoshiHungry &&
        TYoshi::isGreenYoshiAscendingWater(yoshi->mMario))
        animID = 9;

    if ((animID & 0xFFFF) == 24)
        animID = 15;

    return animID;
}

// 0x8026FE84 NEEDS ADDI R4, R3, 0
u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC ||
        !SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
        return player->mState;
    else if (SMEFile::mStageConfig.Yoshi.mYoshiHungry)
        return player->mState;

    if (TYoshi::isGreenYoshiAscendingWater(player))
        return (player->mState & 0xFFFFFBFF) | TMario::State::AIRBORN;
    else
        return player->mState;
}

// 0x8024E788
// extern -> SME.cpp
bool isYoshiValidDrip(TYoshi *yoshi)
{
    return yoshi->isMounted() && !yoshi->isGreenYoshi();
}

// 0x801BC128
// extern -> SME.cpp
void initFreeEggCard(MActorAnmBck *bckData)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
        return;

    bckData->mFrameRate = 11;
}

// 0x801BC380
// extern -> SME.cpp
u32 checkFreeEggCard(MActorAnmBck *bckData)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
        return 0;

    bckData->mFrameRate = 11;
    return 0;
}

// 0x8028121C
// extern -> SME.cpp
void saveNozzles(TYoshi *yoshi)
{
    TMario *player = yoshi->mMario;
    player->mCustomInfo->FluddHistory.mMainNozzle = player->mFludd->mCurrentNozzle;
    player->mCustomInfo->FluddHistory.mSecondNozzle = player->mFludd->mSecondNozzle;
    player->mCustomInfo->FluddHistory.mWaterLevel = player->mFludd->mCurrentWater;
    player->mCustomInfo->FluddHistory.mHadFludd = player->mAttributes.mHasFludd;
    ride__6TYoshiFv(yoshi);
}

// 0x8024EC18
// extern -> SME.cpp
void restoreNozzles(TMario *player)
{
    f32 factor = (f32)player->mCustomInfo->FluddHistory.mWaterLevel / (f32)player->mFludd->mNozzleList[(u8)player->mCustomInfo->FluddHistory.mMainNozzle]->mMaxWater;
    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(player->mFludd, player->mCustomInfo->FluddHistory.mSecondNozzle, 1);
    normalizeNozzle__6TMarioFv(player);
    player->mFludd->mCurrentWater = player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]->mMaxWater * factor;
    player->mAttributes.mHasFludd = player->mCustomInfo->FluddHistory.mHadFludd;
}