#include "includes/eclipse.hxx"

bool isAnyFileComplete(TCardBookmarkInfo* bookmark) {
    for (u32 i = 0; i < 3; ++i) {
        if (bookmark->FileData[i].mShineCount >= MAX_SHINES)
            return true;
    }
    return false;
}

//0x802B1794
u32 setFileCompleteBool(TCardManager* gpCardManager) {
    u32 status = getBookmarkInfos___12TCardManagerFv(gpCardManager);
    if (gpCardManager->mBookMarks) {
        gInfo.mIsCompletionRewards = isAnyFileComplete(gpCardManager->mBookMarks);
    }
    return status;
}
kmCall(0x802B1794, &setFileCompleteBool);

//0x80164DE4
void newGamePlus(TFlagManager* gpFlagManager, u32* stream) {
    TMario* gpMario = (TMario*)*(u32*)TMarioInstance;
    load__12TFlagManagerFR20JSUMemoryInputStream(gpFlagManager, stream);
    if (gpFlagManager->Type4Flag.mShineCount < 120 || (gpFlagManager->Type1Flag.m1Type[0xE] & 0x80) == 0) {
        return;
    }
    else if (gpMario->mController->isPressed(TMarioGamePad::R) && gpMario->mController->isPressed(TMarioGamePad::L)) {
        gpFlagManager->Type6Flag.CustomFlags.mIsGamePlus = true;
        //Type 1
        memset(gpFlagManager->Type1Flag.m1Type, 0, 0x77);
        //Type 2
        gpFlagManager->Type2Flag.mDelfinoCoinRecord = 0;
        gpFlagManager->Type2Flag.mBiancoCoinRecord = 0;
        gpFlagManager->Type2Flag.mRiccoCoinRecord = 0;
        gpFlagManager->Type2Flag.mGelatoCoinRecord = 0;
        gpFlagManager->Type2Flag.mPinnaCoinRecord = 0;
        gpFlagManager->Type2Flag.mSirenaCoinRecord = 0;
        gpFlagManager->Type2Flag.mPiantaCoinRecord = 0;
        gpFlagManager->Type2Flag.mNokiCoinRecord = 0;
        gpFlagManager->Type2Flag.mNoki5Record = 0;

        //Type 3
        gpFlagManager->Type3Flag.mLostLifePrev = false;
        gpFlagManager->Type3Flag.mPlazaDemoWatched = false;
        gpFlagManager->Type3Flag.mWatchPeachKidnappedPrev = false;
        gpFlagManager->Type3Flag.mPpdBJRBalloonsPrev = false;
        gpFlagManager->Type3Flag.mShineGetPrev = false;
        gpFlagManager->Type3Flag.mPlaneCrashWatched = false;
        gpFlagManager->Type3Flag.mCourtWatched = false;
        gpFlagManager->Type3Flag.mPeachKNAPFMVWatched = false;
        gpFlagManager->Type3Flag.mFluddTheftViewed = false;

        //Type 4
        gpFlagManager->Type4Flag.mShineCount = 0;
        gpFlagManager->Type4Flag.mBlueCoinCount = 0;
        gpFlagManager->Type4Flag.mGoldCoinCount = 0;
        gpFlagManager->Type4Flag.mSecondNozzle = TWaterGun::Turbo;

        //Type 5
        gpFlagManager->Type5Flag.mShineSpawned = false;
        gpFlagManager->Type5Flag.mRiccoUnlockable = false;
        gpFlagManager->Type5Flag.mGelatoUnlockable = false;
        gpFlagManager->Type5Flag.mSunflowersRescue = false;
        gpFlagManager->Type5Flag.mNokiAvailable = false;
        gpFlagManager->Type5Flag.mPiantismoRaceComplete = false;
        gpFlagManager->Type5Flag.mMantaSpawned = false;
        gpFlagManager->Type5Flag.mHotelRising = false;
        gpFlagManager->Type5Flag.mRedCoinSwitchPressed = false;
        gpFlagManager->Type5Flag.mMechaBowserDefeated = false;
        gpFlagManager->Type5Flag.mWigglerFalling = false;
        gpFlagManager->Type5Flag.mMoleDefeated = false;

        //Type 6
        gpFlagManager->Type6Flag.mRedCoinCount = 0;
        gpFlagManager->Type6Flag.mShadowMarioEvent = 0;
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

void isGPlusNozzleBox(TNozzleBox* gpNozzleBox) {
    TFlagManager* gpFlagManager = (TFlagManager*)*(u32*)TFlagManagerInstance;
    if (gpFlagManager->Type6Flag.CustomFlags.mIsGamePlus == false) {
        return;
    }
    if (gpNozzleBox->mNozzleToRegen == TWaterGun::Hover) {
        makeObjDead__11TMapObjBaseFv(gpNozzleBox);
    }
}