#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "SME.hxx"

/* 0x8026A164
mr r11, r5
lwz r5, 0x8 (r3)
lwz r5, 0xC8 (r5)
lwz r5, 0 (r5)
cmpwi r5, NULLPTR
beq exit

addi r6, r5, 0x34
lbzx r6, r6, r4
cmpwi r6, FALSE
bne exit

lbz r4, 0x1C84 (r3)

exit:
mr r5, r11
mflr r0
*/

/*0x8026A164
TWaterGun::NozzleType changeNozzleIfSet(TWaterGun* gpFludd, TWaterGun::NozzleType nozzle, bool normalize) {
    if (gpFludd->mMario->mCustomInfo->mParams) {
        if (!gpFludd->mMario->mCustomInfo->mParams->Attributes.FluddAttrs.mCanUseNozzle[(u8)nozzle]) {
            nozzle = gpFludd->mCurrentNozzle;
        }
    } 
    return nozzle; //return gpFludd and normalize too
    //mflr r0
}
*/

static bool sIsTriggerNozzleDead;

// 0x80248F14
// extern -> SME.cpp
bool isPumpOK(TMarioAnimeData *animeData)
{
    return (animeData->mFluddEnabled != TMarioAnimeData::FLUDD::FLUDD_DISABLED &&
            gpMarioAddress->mCustomInfo->mCurJump <= 1);
}

// 0x8014206C
// extern -> SME.cpp
bool hasWaterCardOpen()
{
    TGCConsole2 *gcConsole;
    SME_FROM_GPR(r31, gcConsole);

    if (gpMarioAddress->mYoshi->mState != TYoshi::State::MOUNTED &&
        gpMarioAddress->mCustomInfo->mParams)
        gpMarioAddress->mAttributes.mHasFludd =
            gpMarioAddress->mCustomInfo->mParams->Attributes.mCanUseFludd;
    else
        gpMarioAddress->mAttributes.mHasFludd = true;

    if (gpMarioAddress->mYoshi->mState != TYoshi::State::MOUNTED &&
        !gpMarioAddress->mAttributes.mHasFludd &&
        !gcConsole->mWaterCardFalling &&
        gcConsole->mIsWaterCard)
        startDisappearTank__11TGCConsole2Fv(gcConsole);

    return gcConsole->mIsWaterCard;
}

// 0x80283058
// extern -> SME.cpp
bool canCollectFluddItem(TMario *player)
{
    if (player->mCustomInfo->mParams)
        return onYoshi__6TMarioCFv(player) ||
               !player->mCustomInfo->mParams->Attributes.mCanUseFludd;
    else
        return onYoshi__6TMarioCFv(player);
}

static void sprayGoopMap(TPollutionManager *gpPollutionManager, TMario *player,
                         f32 x, f32 y, f32 z, f32 r)
{
    if (!player->mCustomInfo->isMario() || !player->mCustomInfo->isInitialized())
        clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);

    MarioParamsFile *localfile = player->mCustomInfo->mParams;
    if (localfile && localfile->Attributes.FluddAttrs.mCleaningType != localfile->NONE)
    {
        if (localfile->Attributes.FluddAttrs.mCleaningType == localfile->CLEAN)
            clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
        else if (localfile->Attributes.FluddAttrs.mCleaningType == localfile->GOOP)
            stamp__17TPollutionManagerFUsffff(gpPollutionManager, 1, x, y, z, r);
    }
}

// 0x800678C4, 0x801A3ED0, 0x801B42D8, 0x8027F7DC, 0x8027F94C,
// extern -> SME.cpp
void sprayGoopMapWrapGlobalMar(TPollutionManager *gpPollutionManager,
                               f32 x, f32 y, f32 z, f32 r)
{
    sprayGoopMap(gpPollutionManager, gpMarioAddress, x, y, z, r);
}

// 0x8024E710
// extern -> SME.cpp
void sprayGoopMapWrapMar30(TPollutionManager *gpPollutionManager,
                           f32 x, f32 y, f32 z, f32 r)
{
    TMario *player;
    SME_FROM_GPR(r30, player);

    sprayGoopMap(gpPollutionManager, player, x, y, z, r);
}

// 0x800FED3C
// extern -> SME.cpp
bool canCleanSeals(TWaterManager *gpWaterManager)
{
    MarioParamsFile *localfile = gpMarioAddress->mCustomInfo->mParams;
    return gpWaterManager->mWaterCardType != 0 ||
           (localfile && localfile->Attributes.FluddAttrs.mCanCleanSeals);
}

// 0x8024D53C
// extern -> SME.cpp
TWaterGun *bindFluddtojoint()
{
    TMario *player;
    SME_FROM_GPR(r31, player);

    MarioParamsFile *localfile = player->mCustomInfo->mParams;

    if (localfile)
        player->mBindBoneIDArray[1] =
            localfile->Attributes.FluddAttrs.mBindToJointID[(u8)player->mFludd->mCurrentNozzle];

    return player->mFludd;
}

// 0x80262580
// extern -> SME.cpp
void checkExecWaterGun(TWaterGun *fludd)
{
    if (fludd->mCurrentNozzle == TWaterGun::NozzleType::Spray ||
        fludd->mCurrentNozzle == TWaterGun::NozzleType::Yoshi ||
        fludd->mCurrentNozzle == TWaterGun::NozzleType::Underwater)
    {
        emit__9TWaterGunFv(fludd);
        return;
    }

    if (!sIsTriggerNozzleDead)
        emit__9TWaterGunFv(fludd);
}

// 0x80262580
// extern -> SME.cpp
void killTriggerNozzle()
{
    TNozzleTrigger *nozzle;
    SME_FROM_GPR(r29, nozzle);

    nozzle->mSprayState = TNozzleTrigger::DEAD;
    if (nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Hover ||
        nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Rocket ||
        nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Turbo)
        sIsTriggerNozzleDead = true;
    
}

static void checkSpamHover(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    TMario *player = nozzle->mFludd->mMario;

    if (nozzle->mFludd->mCurrentNozzle != TWaterGun::Hover)
        return;

    nozzle->mEmitPerFrame = 1.0f;
    nozzle->mEmitRandom = 0.0f;

    if ((nozzle->mMaxSprayQuarterFrames - nozzle->mSprayQuarterFramesLeft) >= 20)
        return;

    if (!(player->mControllerWork->mAnalogR < 0.9f) ||
        !player->mControllerWork->mFrameInput & TMarioControllerWork::A)
        return;

    if (nozzle->mFludd->mCurrentWater < 510)
        return;

    nozzle->mEmitPerFrame = 255.0f;
    nozzle->mEmitRandom = 0.5f;
    nozzle->mSprayQuarterFramesLeft = 0;
    nozzle->mSprayState = TNozzleTrigger::DEAD;

    nozzle->mFludd->mCurrentWater -= 255;
    player->mSpeed.y += ((70.0f * player->mSize.y) - player->mSpeed.y);
    player->mJumpingState &= 0xFFFFFEFF;

    sIsTriggerNozzleDead = true;
    return;
}

// 0x80262580
// extern -> SME.cpp
void spamHoverWrapper(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    void (*virtualFunc)(TNozzleTrigger *, u32, TWaterEmitInfo *);
    SME_FROM_GPR(r12, virtualFunc);

    checkSpamHover(nozzle, r4, emitInfo);
    virtualFunc(nozzle, r4, emitInfo);
}

// 0x80262580
// extern -> SME.cpp
bool checkAirNozzle()
{
    TMario *player;
    SME_FROM_GPR(r31, player);
    
    sIsTriggerNozzleDead &= (player->mState & TMario::State::AIRBORN) != 0;

    if (player->mFludd->mCurrentNozzle == TWaterGun::Spray || player->mFludd->mCurrentNozzle == TWaterGun::Yoshi || player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
        return player->mState != TMario::State::HOVER_F;

    return (!(player->mState & TMario::State::AIRBORN) || !sIsTriggerNozzleDead);
}