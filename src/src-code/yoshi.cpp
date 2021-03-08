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
    if (!(player->mAttributes.mIsWater) ||
        !player->mController->isPressed(TMarioGamePad::A) ||
        !player->mYoshi->isGreenYoshiMounted())
    {
        return false;
    }
    else
    {
        return true;
    }
}

kmWrite32(0x8026E068, 0x2C000001); //Turn green when out of juice
kmWrite32(0x8026E0A0, 0x60000000); //No flickering
kmWrite32(0x8026EE14, 0x4800020C); //Yoshi can't starve

//--------Water--Volatility--------//

bool isYoshiDie(TMario *player)
{
    return !player->mYoshi->isGreenYoshi();
}

kmCall(0x8026EB00, &isYoshiDie);
kmWrite32(0x8026EB04, 0x2C030000);
kmWrite32(0x8026EB08, 0x41820518);

kmCall(0x8026EBFC, &isYoshiDie);
kmWrite32(0x8026EC00, 0x2C030000);
kmWrite32(0x8026EC04, 0x4182041C);

kmCall(0x8026F218, &isYoshiDie);
kmWrite32(0x8026F21C, 0x2C030000);
kmWrite32(0x8026F220, 0x41820164);

//0x801BC868
bool isYoshiEggNeedFruit(THitActor *gpFruit)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
    {
        return isFruit__11TMapObjBaseFP9THitActor(gpFruit);
    }
    return true;
}
kmCall(0x801BC868, &isYoshiEggNeedFruit);

//0x801BC8B4
u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit)
{
    if (gpEgg->mState == 14 || gpEgg->mState == 6)
    {
        return 0;
    }
    else if (gpMarioAddress->mCustomInfo->mParams && !gpMarioAddress->mCustomInfo->mParams->Attributes.mCanRideYoshi)
    {
        return 0;
    }
    else if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
    {
        if (gpEgg->mWantedFruit != gpFruit->mObjectID)
            return 2;
        else
            return 1;
    }
    else
    {
        return 1;
    }
}
kmCall(0x801BC8B4, &isYoshiEggFree);
kmWrite32(0x801BC8B8, 0xA01E00FC);
kmWrite32(0x801BC8BC, 0x2C00000B);
kmWrite32(0x801BC8C0, 0x418200E4);
kmWrite32(0x801BC8C4, 0x2C030001);
kmWrite32(0x801BC8C8, 0x4182003C);
kmWrite32(0x801BC8CC, 0x418100D8);
kmWrite32(0x801BC8D0, 0x48000134);

//0x8024D68C
bool isYoshiMaintainFluddModel()
{
    TMario *player;
    __asm { mr player, r31 };

    if (player->mYoshi->mState == TYoshi::MOUNTED)
        return (player->mCustomInfo->FluddHistory.mHadFludd & player->mAttributes.mHasFludd);
    else
        return player->mAttributes.mHasFludd;
}
kmCall(0x8024D68C, &isYoshiMaintainFluddModel);
kmWrite32(0x8024D690, 0x2C030000);

//0x8024F240
void isYoshiDrown(TYoshi *yoshi)
{
    if (!yoshi->isGreenYoshi())
    {
        disappear__6TYoshiFv(yoshi);
    }
}
kmCall(0x8024F240, &isYoshiDrown);

bool canMountYoshi(u32 state, JGeometry::TVec3<f32> *yoshiCoordinates)
{
    TMario *player;
    __asm { mr player, r31 };

    if (player->mCustomInfo->mParams)
    {
        if (state & TMario::SA_WATERBORN)
            return player->mCustomInfo->mParams->Attributes.mCanRideYoshi;
        else
            return ((state & TMario::SA_AIRBORN) && player->mCustomInfo->mParams->Attributes.mCanRideYoshi);
    }
    else
    {
        return state & TMario::SA_AIRBORN;
    }
}

//0x802810F8
bool canMountYoshiWrapper(u32 state, JGeometry::TVec3<f32> *yoshiCoordinates)
{
    bool canMount = canMountYoshi(state, yoshiCoordinates);
    __asm volatile { mr r4, yoshiCoordinates };

    return canMount;
}
kmCall(0x802810F8, &canMountYoshiWrapper);
kmWrite32(0x802810FC, 0x2C030000);

//0x8026E810
void fixYoshiJuiceDecrement()
{
    TYoshi *yoshi;
    __asm { mr yoshi, r30 };

    TMario *player = yoshi->mMario;
    if (player->mFludd->mIsEmitWater && yoshi->isMounted())
    {
        yoshi->mCurJuice -= player->mFludd->mEmitInfo->mEmitCount;
    }
}
kmCall(0x8026E810, &fixYoshiJuiceDecrement);

//0x8024E58C
void canYoshiSpray(TWaterGun *gpWaterGun)
{
    TMario *player = gpWaterGun->mMario;
    if (!player->mYoshi)
        return;

    if (!player->mYoshi->isGreenYoshiMounted())
    {
        emit__9TWaterGunFv(gpWaterGun);
    }
}
kmCall(0x8024E58C, &canYoshiSpray);

//0x80273198
u32 calcYoshiSwimVelocity(TMario *player, u32 arg1)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
    {
        return jumpProcess__6TMarioFi(player, arg1);
    }
    else if (SMEFile::mStageConfig.Yoshi.mYoshiHungry)
    {
        return jumpProcess__6TMarioFi(player, arg1);
    }

    if (!player->mYoshi)
        return jumpProcess__6TMarioFi(player, arg1);

    if (!player->mYoshi->isGreenYoshiMounted())
        return jumpProcess__6TMarioFi(player, arg1);

    if (player->mController->isPressed(TMarioGamePad::A))
    {
        if (player->mCustomInfo->mYoshiWaterSpeed.y > 12)
        {
            player->mCustomInfo->mYoshiWaterSpeed.y = 12;
        }
        else
        {
            player->mCustomInfo->mYoshiWaterSpeed.y += 0.34375;
        }
    }
    else
    {
        if (player->mCustomInfo->mYoshiWaterSpeed.y < -12)
        {
            player->mCustomInfo->mYoshiWaterSpeed.y = -12;
        }
        else
        {
            player->mCustomInfo->mYoshiWaterSpeed.y -= 0.34375;
        }
    }
    player->mSpeed.y = player->mCustomInfo->mYoshiWaterSpeed.y;
    return jumpProcess__6TMarioFi(player, arg1);
}
kmCall(0x80273198, &calcYoshiSwimVelocity);

//0x80270078
u32 isYoshiWaterFlutter()
{
    TYoshi *yoshi;
    u32 animID;

    __asm { mr yoshi, r29 };
    __asm { mr animID, r30 };

    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC && SMEFile::mStageConfig.GlobalFlags.mIsYoshi && SMEFile::mStageConfig.Yoshi.mYoshiHungry && TYoshi::isGreenYoshiAscendingWater(yoshi->mMario))
    {
        animID = 9;
    }

    if ((animID & 0xFFFF) == 24)
        animID = 15;

    return animID;
}
kmCall(0x80270078, &isYoshiWaterFlutter);
kmWrite32(0x8027007C, 0x7C7E1B78);
kmWrite32(0x80270080, 0x60000000);
kmWrite32(0x80270084, 0x60000000);

//0x8026FE84 NEEDS ADDI R4, R3, 0
u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
    {
        return player->mState;
    }
    else if (SMEFile::mStageConfig.Yoshi.mYoshiHungry)
    {
        return player->mState;
    }

    if (TYoshi::isGreenYoshiAscendingWater(player))
    {
        return player->mState & 0xFFFFFBFF | TMario::SA_AIRBORN;
    }
    else
    {
        return player->mState;
    }
}

//0x8024E788
bool isYoshiValidDrip(TYoshi *yoshi)
{
    return yoshi->isMounted() && !yoshi->isGreenYoshi();
}
kmCall(0x8024E788, &isYoshiValidDrip);

//0x801BC128
void initFreeEggCard(MActorAnmBck *bckData)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
        return;

    bckData->mFrameRate = 11;
}
kmBranch(0x801BC128, &initFreeEggCard);

//0x801BC380
u32 checkFreeEggCard(MActorAnmBck *bckData)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi || !SMEFile::mStageConfig.Yoshi.mIsEggFree)
        return 0;

    bckData->mFrameRate = 11;
    return 0;
}
kmCall(0x801BC380, &checkFreeEggCard);

//0x8028121C
void saveNozzles(TYoshi *yoshi)
{
    TMario *player = yoshi->mMario;
    player->mCustomInfo->FluddHistory.mMainNozzle = player->mFludd->mCurrentNozzle;
    player->mCustomInfo->FluddHistory.mSecondNozzle = player->mFludd->mSecondNozzle;
    player->mCustomInfo->FluddHistory.mWaterLevel = player->mFludd->mCurrentWater;
    player->mCustomInfo->FluddHistory.mHadFludd = player->mAttributes.mHasFludd;
    ride__6TYoshiFv(yoshi);
}
kmCall(0x8028121C, &saveNozzles);

//0x8024EC18
void restoreNozzles(TMario *player)
{
    f32 factor = (f32)player->mCustomInfo->FluddHistory.mWaterLevel / (f32)player->mFludd->mNozzleList[(u8)player->mCustomInfo->FluddHistory.mMainNozzle]->mMaxWater;
    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(player->mFludd, player->mCustomInfo->FluddHistory.mSecondNozzle, 1);
    normalizeNozzle__6TMarioFv(player);
    player->mFludd->mCurrentWater = player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]->mMaxWater * factor;
    player->mAttributes.mHasFludd = player->mCustomInfo->FluddHistory.mHadFludd;
}
kmCall(0x8024EC18, &restoreNozzles);

kmWrite32(0x8024EC2C, 0x60000000);