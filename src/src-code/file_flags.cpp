#include "sms/object/NozzleBox.hxx"

#include "SME.hxx"

static bool isAnyFileComplete(TCardBookmarkInfo *bookmark)
{
    for (u32 i = 0; i < 3; ++i)
    {
        if (bookmark->FileData[i].mShineCount >= SME_MAX_SHINES)
            return true;
    }
    return false;
}

// 0x802B1794
// extern -> SME.cpp
s32 setFileCompleteBool(TCardManager *gpCardManager)
{
    s32 ret = getBookmarkInfos___12TCardManagerFv(gpCardManager);
    if (gpCardManager->mBookMarks)
        gInfo.Context.mIsCompletionRewards = isAnyFileComplete(gpCardManager->mBookMarks);
    return ret;
}

//0x80164DE4
void newGamePlus(TFlagManager *flagManager, JSUMemoryInputStream &stream)
{
    flagManager->load(stream);
    if (flagManager->Type4Flag.mShineCount < 120 || (flagManager->Type1Flag.m1Type[0xE] & 0x80) == 0)
        return;

    else if (gpMarioAddress->mController->mButtons.mInput & TMarioGamePad::Buttons::R &&
             gpMarioAddress->mController->mButtons.mInput & TMarioGamePad::Buttons::L)
    {
        flagManager->Type6Flag.CustomFlags.mIsGamePlus = true;
        //Type 1
        memset(flagManager->Type1Flag.m1Type, 0, 0x77);
        //Type 2
        flagManager->Type2Flag.mDelfinoCoinRecord = 0;
        flagManager->Type2Flag.mBiancoCoinRecord = 0;
        flagManager->Type2Flag.mRiccoCoinRecord = 0;
        flagManager->Type2Flag.mGelatoCoinRecord = 0;
        flagManager->Type2Flag.mPinnaCoinRecord = 0;
        flagManager->Type2Flag.mSirenaCoinRecord = 0;
        flagManager->Type2Flag.mPiantaCoinRecord = 0;
        flagManager->Type2Flag.mNokiCoinRecord = 0;
        flagManager->Type2Flag.mNoki5Record = 0;

        //Type 3
        flagManager->Type3Flag.mLostLifePrev = false;
        flagManager->Type3Flag.mPlazaDemoWatched = false;
        flagManager->Type3Flag.mWatchPeachKidnappedPrev = false;
        flagManager->Type3Flag.mPpdBJRBalloonsPrev = false;
        flagManager->Type3Flag.mShineGetPrev = false;
        flagManager->Type3Flag.mPlaneCrashWatched = false;
        flagManager->Type3Flag.mCourtWatched = false;
        flagManager->Type3Flag.mPeachKNAPFMVWatched = false;
        flagManager->Type3Flag.mFluddTheftViewed = false;

        //Type 4
        flagManager->Type4Flag.mShineCount = 0;
        flagManager->Type4Flag.mBlueCoinCount = 0;
        flagManager->Type4Flag.mGoldCoinCount = 0;
        flagManager->Type4Flag.mSecondNozzle = TWaterGun::Turbo;

        //Type 5
        flagManager->Type5Flag.mShineSpawned = false;
        flagManager->Type5Flag.mRiccoUnlockable = false;
        flagManager->Type5Flag.mGelatoUnlockable = false;
        flagManager->Type5Flag.mSunflowersRescue = false;
        flagManager->Type5Flag.mNokiAvailable = false;
        flagManager->Type5Flag.mPiantismoRaceComplete = false;
        flagManager->Type5Flag.mMantaSpawned = false;
        flagManager->Type5Flag.mHotelRising = false;
        flagManager->Type5Flag.mRedCoinSwitchPressed = false;
        flagManager->Type5Flag.mMechaBowserDefeated = false;
        flagManager->Type5Flag.mWigglerFalling = false;
        flagManager->Type5Flag.mMoleDefeated = false;

        //Type 6
        flagManager->Type6Flag.mRedCoinCount = 0;
        flagManager->Type6Flag.mShadowMarioEvent = 0;
    }
}

//GPlus health
//0x80243838
/*
lis r5, 0x8041
lwz r5, -0x1EA0(r5)
lwz r5, 0x27C(r5)
rlwinm r5,r5,1,31,31
slw r4, r4, r5
mr r31, r4
*/

//Force no hover
//0x8026A180
/*
lis r6, 0x8041
lwz r6, -0x1EA0(r6)
lwz r6, 0x27C(r6)
andis. r6, r6, 0x8000
beq+ pass
cmpwi r4, 4
bne+ pass
li r4, 5
pass:
mr. r30, r4
*/

//Init with turbo
//0x8026A3C8
/*
li r8, 4
lis r7, 0x8041
lwz r7, -0x1EA0(r7)
lwz r7, 0x27C(r7)
andis. r7, r7, 0x8000
beq+ pass
li r8, 5
pass:
*/

//0x801BB660
/*
.loc_0x0:
  lis r4, 0x8041
  lwz r4, -0x1EA0(r4)
  lwz r4, 0x27C(r4)
  andis. r4, r4, 0x8000
  beq- .loc_0x34
  lwz r4, 0x148(r31)
  cmplwi r4, 0x4
  bne- .loc_0x34
  mr r3, r31
  lis r4, 0x801B
  ori r4, r4, 0x738
  mtctr r4
  bctrl 

.loc_0x34:
  lwz r0, 0x4C(sp)
*/

void isGPlusNozzleBox(TNozzleBox *gpNozzleBox)
{
    if (!TFlagManager::smInstance->Type6Flag.CustomFlags.mIsGamePlus)
        return;

    if (gpNozzleBox->mNozzleToRegen == TWaterGun::Hover)
        makeObjDead__11TMapObjBaseFv(gpNozzleBox);
}