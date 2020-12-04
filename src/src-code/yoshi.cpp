#include "includes/yoshi.hxx"
extern CustomInfo gInfo;

kmWrite32(0x8026E068, 0x2C000001); //Turn green when out of juice
kmWrite32(0x8026E0A0, 0x60000000); //No flickering
kmWrite32(0x8026EE14, 0x4800020C); //Yoshi can't starve

//--------Water--Volatility--------//

bool isYoshiDie(TMario *gpMario)
{
    return !gpMario->mYoshi->isGreenYoshi();
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
    if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi || !gInfo.mFile->Yoshi.mIsEggFree)
    {
        return isFruit__11TMapObjBaseFP9THitActor(gpFruit);
    }
    return true;
}
kmCall(0x801BC868, &isYoshiEggNeedFruit);

//0x801BC8B4
u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpEgg->mState == 14 || gpEgg->mState == 6)
    {
        return 0;
    }
    else if (gpMario->mCustomInfo->mParams && !gpMario->mCustomInfo->mParams->Attributes.mCanRideYoshi)
    {
        return 0;
    }
    else if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi || !gInfo.mFile->Yoshi.mIsEggFree)
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
    register TMario *gpMario;
    __asm { mr gpMario, r31 };

    if (gpMario->mYoshi->mState == TYoshi::MOUNTED)
        return (gpMario->mCustomInfo->FluddHistory.mHadFludd & gpMario->mAttributes.mHasFludd);
    else
        return gpMario->mAttributes.mHasFludd;
}
kmCall(0x8024D68C, &isYoshiMaintainFluddModel);
kmWrite32(0x8024D690, 0x2C030000);

//0x8024F240
void isYoshiDrown(TYoshi *gpYoshi)
{
    if (!gpYoshi->isGreenYoshi())
    {
        disappear__6TYoshiFv(gpYoshi);
    }
}
kmCall(0x8024F240, &isYoshiDrown);

bool canMountYoshi(u32 state, JGeometry::TVec3<float> *yoshiCoordinates)
{
    register TMario *gpMario;
    __asm { mr gpMario, r31 };

    if (gpMario->mCustomInfo->mParams)
    {
        if (state & TMario::SA_WATERBORN)
            return gpMario->mCustomInfo->mParams->Attributes.mCanRideYoshi;
        else
            return ((state & TMario::SA_AIRBORN) && gpMario->mCustomInfo->mParams->Attributes.mCanRideYoshi);
    }
    else
    {
        return state & TMario::SA_AIRBORN;
    }
}

//0x802810F8
bool canMountYoshiWrapper(u32 state, JGeometry::TVec3<float> *yoshiCoordinates)
{
    register JGeometry::TVec3<float> *tmp;
    __asm { mr tmp, r4 };

    bool canMount = canMountYoshi(state, yoshiCoordinates);
    __asm { mr r4, tmp };
    return canMount;
}
kmCall(0x802810F8, &canMountYoshiWrapper);
kmWrite32(0x802810FC, 0x2C030000);

//0x8026E810
void fixYoshiJuiceDecrement()
{
    register TYoshi *gpYoshi;
    __asm { mr gpYoshi, r30 };

    TMario *gpMario = gpYoshi->mMario;
    if (gpMario->mFludd->mIsEmitWater && gpYoshi->isMounted())
    {
        gpYoshi->mCurJuice -= gpMario->mFludd->mEmitInfo[0x18 / 4];
    }
}
kmCall(0x8026E810, &fixYoshiJuiceDecrement);

//0x8024E58C
void canYoshiSpray(TWaterGun *gpWaterGun)
{
    TMario *gpMario = gpWaterGun->mMario;
    if (!gpMario->mYoshi)
        return;

    if (!gpMario->mYoshi->isGreenYoshiMounted())
    {
        emit__9TWaterGunFv(gpWaterGun);
    }
}
kmCall(0x8024E58C, &canYoshiSpray);

//0x80273198
u32 calcYoshiSwimVelocity(TMario *gpMario, u32 arg1)
{

    if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi)
    {
        return jumpProcess__6TMarioFi(gpMario, arg1);
    }
    else if (gInfo.mFile->Yoshi.mYoshiHungry)
    {
        return jumpProcess__6TMarioFi(gpMario, arg1);
    }

    if (!gpMario->mYoshi)
        return jumpProcess__6TMarioFi(gpMario, arg1);

    if (!gpMario->mYoshi->isGreenYoshiMounted())
        return jumpProcess__6TMarioFi(gpMario, arg1);

    if (gpMario->mController->isPressed(TMarioGamePad::A))
    {
        if (gInfo.Mario.yoshiWaterSpeed.y > 12)
        {
            gInfo.Mario.yoshiWaterSpeed.y = 12;
        }
        else
        {
            gInfo.Mario.yoshiWaterSpeed.y += 0.34375;
        }
    }
    else
    {
        if (gInfo.Mario.yoshiWaterSpeed.y < -12)
        {
            gInfo.Mario.yoshiWaterSpeed.y = -12;
        }
        else
        {
            gInfo.Mario.yoshiWaterSpeed.y -= 0.34375;
        }
    }
    gpMario->mSpeed.y = gInfo.Mario.yoshiWaterSpeed.y;
    return jumpProcess__6TMarioFi(gpMario, arg1);
}
kmCall(0x80273198, &calcYoshiSwimVelocity);

//0x80270078
void isYoshiWaterFlutter()
{
    register TYoshi *gpYoshi;
    register int animID;

    __asm { mr gpYoshi, r29 };
    __asm { mr animID, r30 };

    if (gInfo.mFile && gInfo.mFile->GlobalFlags.mIsYoshi && gInfo.mFile->Yoshi.mYoshiHungry && TYoshi::isGreenYoshiAscendingWater(gpYoshi->mMario))
    {
        animID = 9;
    }
    __asm { mr r30, animID };
    __asm { clrlwi r0, animID, 16 };
}
kmCall(0x80270078, &isYoshiWaterFlutter);

//0x8026FE84 NEEDS ADDI R4, R3, 0
u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *gpMario)
{
    if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi)
    {
        return gpMario->mState;
    }
    else if (gInfo.mFile->Yoshi.mYoshiHungry)
    {
        return gpMario->mState;
    }

    if (TYoshi::isGreenYoshiAscendingWater(gpMario))
    {
        return gpMario->mState & 0xFFFFFBFF | TMario::SA_AIRBORN;
    }
    else
    {
        return gpMario->mState;
    }
}

//0x8024E788
bool isYoshiValidDrip(TYoshi *gpYoshi)
{
    return gpYoshi->isMounted() && !gpYoshi->isGreenYoshi();
}
kmCall(0x8024E788, &isYoshiValidDrip);

//0x801BC128
void initFreeEggCard(MActorAnmBck *bckData)
{
    if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi || !gInfo.mFile->Yoshi.mIsEggFree)
        return;

    bckData->mFrameRate = 11;
}
kmBranch(0x801BC128, &initFreeEggCard);

//0x801BC380
u32 checkFreeEggCard(MActorAnmBck *bckData)
{
    if (!gInfo.mFile || !gInfo.mFile->GlobalFlags.mIsYoshi || !gInfo.mFile->Yoshi.mIsEggFree)
        return 0;

    bckData->mFrameRate = 11;
    return 0;
}
kmCall(0x801BC380, &checkFreeEggCard);

//0x8028121C
void saveNozzles(TYoshi *gpYoshi)
{
    TMario *gpMario = gpYoshi->mMario;
    gpMario->mCustomInfo->FluddHistory.mMainNozzle = gpMario->mFludd->mCurrentNozzle;
    gpMario->mCustomInfo->FluddHistory.mSecondNozzle = gpMario->mFludd->mSecondNozzle;
    gpMario->mCustomInfo->FluddHistory.mWaterLevel = gpMario->mFludd->mCurrentWater;
    gpMario->mCustomInfo->FluddHistory.mHadFludd = gpMario->mAttributes.mHasFludd;
    ride__6TYoshiFv(gpYoshi);
}
kmCall(0x8028121C, &saveNozzles);

//0x8024EC18
void restoreNozzles(TMario *gpMario)
{
    float factor = (float)gpMario->mCustomInfo->FluddHistory.mWaterLevel / (float)gpMario->mFludd->mNozzleList[(u8)gpMario->mCustomInfo->FluddHistory.mMainNozzle]->mMaxWater;
    changeNozzle__9TWaterGunFQ29TWaterGun11TNozzleTypeb(gpMario->mFludd, gpMario->mCustomInfo->FluddHistory.mSecondNozzle, 1);
    normalizeNozzle__6TMarioFv(gpMario);
    gpMario->mFludd->mCurrentWater = gpMario->mFludd->mNozzleList[(u8)gpMario->mFludd->mCurrentNozzle]->mMaxWater * factor;
    gpMario->mAttributes.mHasFludd = gpMario->mCustomInfo->FluddHistory.mHadFludd;
}
kmCall(0x8024EC18, &restoreNozzles);

kmWrite32(0x8024EC2C, 0x60000000);