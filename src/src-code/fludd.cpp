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
TWaterGun::NOZZLETYPE changeNozzleIfSet(TWaterGun* gpFludd, TWaterGun::NOZZLETYPE nozzle, bool normalize) {
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

//0x80248F14
static bool isPumpOK(TMarioAnimeData *animeData)
{
    return (animeData->mFluddEnabled != animeData->FLUDD_DISABLED && gpMarioAddress->mCustomInfo->mCurJump <= 1);
}
kmBranch(0x80248F14, &isPumpOK);
kmWrite16(0x803DCA02, TMarioAnimeData::FLUDD_ENABLED);

//0x8014206C
static bool hasWaterCardOpen()
{
    TGCConsole2 *gcConsole;
    asm volatile ( "mr %0, r31" : "=r"(gcConsole));

    if (gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED && gpMarioAddress->mCustomInfo->mParams)
    {
        gpMarioAddress->mAttributes.mHasFludd = gpMarioAddress->mCustomInfo->mParams->Attributes.mCanUseFludd;
    }
    else
    {
        gpMarioAddress->mAttributes.mHasFludd = true;
    }

    if (gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED &&
        !gpMarioAddress->mAttributes.mHasFludd &&
        !gcConsole->mWaterCardFalling &&
        gcConsole->mIsWaterCard)
    {
        startDisappearTank__11TGCConsole2Fv(gcConsole);
    }

    return gcConsole->mIsWaterCard;
}
kmCall(0x8014206C, &hasWaterCardOpen);
kmWrite32(0x80142070, 0x28030000);

//0x80283058
static bool canCollectFluddItem(TMario *player)
{
    if (player->mCustomInfo->mParams)
        return (onYoshi__6TMarioCFv(player) || !player->mCustomInfo->mParams->Attributes.mCanUseFludd);
    else
        return onYoshi__6TMarioCFv(player);
}
kmCall(0x80283058, &canCollectFluddItem);

//0x8024E710
static void sprayGoopMap(TPollutionManager *gpPollutionManager, TMario *player, f32 x, f32 y, f32 z, f32 r)
{
    if (!player->mCustomInfo->isMario() || !player->mCustomInfo->isInitialized())
        clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);

    MarioParamsFile *localfile = player->mCustomInfo->mParams;
    if (localfile && localfile->Attributes.FluddAttrs.mCleaningType != localfile->NONE)
    {
        if (localfile->Attributes.FluddAttrs.mCleaningType == localfile->CLEAN)
        {
            clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
        }
        else if (localfile->Attributes.FluddAttrs.mCleaningType == localfile->GOOP)
        {
            stamp__17TPollutionManagerFUsffff(gpPollutionManager, 1, x, y, z, r);
        }
    }
}

static void sprayGoopMapWrapGlobalMar(TPollutionManager *gpPollutionManager, f32 x, f32 y, f32 z, f32 r)
{
    sprayGoopMap(gpPollutionManager, gpMarioAddress, x, y, z, r);
}

static void sprayGoopMapWrapMar30(TPollutionManager *gpPollutionManager, f32 x, f32 y, f32 z, f32 r)
{
    TMario *player;
    asm volatile ( "mr %0, r30" : "=r"(player));

    sprayGoopMap(gpPollutionManager, player, x, y, z, r);
}

kmCall(0x800678C4, &sprayGoopMapWrapGlobalMar);
kmCall(0x801A3ED0, &sprayGoopMapWrapGlobalMar);
kmCall(0x801B42D8, &sprayGoopMapWrapGlobalMar);
kmCall(0x8024E710, &sprayGoopMapWrapMar30);
kmCall(0x8027F7DC, &sprayGoopMapWrapGlobalMar);
kmCall(0x8027F94C, &sprayGoopMapWrapGlobalMar);

//0x800FED3C
static bool canCleanSeals(TWaterManager *gpWaterManager)
{
    MarioParamsFile *localfile = gpMarioAddress->mCustomInfo->mParams;
    return (gpWaterManager->mWaterCardType != 0 || (localfile && localfile->Attributes.FluddAttrs.mCanCleanSeals));
}
kmCall(0x800FED3C, &canCleanSeals);
kmWrite32(0x800FED40, 0x2C030000);

//0x8024D53C
static TWaterGun *bindFluddtojoint()
{
    TMario *player;
    asm volatile ( "mr %0, r31" : "=r"(player));

    MarioParamsFile *localfile = player->mCustomInfo->mParams;

    if (localfile)
    {
        player->mBindBoneIDArray[1] = localfile->Attributes.FluddAttrs.mBindToJointID[(u8)player->mFludd->mCurrentNozzle];
    }
    return player->mFludd;
}
kmCall(0x8024D53C, &bindFluddtojoint);

static void checkExecWaterGun(TWaterGun *fludd)
{
    if (fludd->mCurrentNozzle == TWaterGun::Spray || fludd->mCurrentNozzle == TWaterGun::Yoshi || fludd->mCurrentNozzle == TWaterGun::Underwater)
    {
        emit__9TWaterGunFv(fludd);
        return;
    }

    if (!sIsTriggerNozzleDead)
    {
        emit__9TWaterGunFv(fludd);
    }
}
kmCall(0x8024E548, &checkExecWaterGun);

static void killTriggerNozzle()
{
    TNozzleTrigger *nozzle;
    asm volatile ( "mr %0, r29" : "=r"(nozzle));

    nozzle->mSprayState = TNozzleTrigger::DEAD;
    if (nozzle->mFludd->mCurrentNozzle == TWaterGun::Hover || nozzle->mFludd->mCurrentNozzle == TWaterGun::Rocket || nozzle->mFludd->mCurrentNozzle == TWaterGun::Turbo)
    {
        sIsTriggerNozzleDead = true;
    }
    
}
kmCall(0x8026C370, &killTriggerNozzle);

static void checkSpamHover(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    TMario *player = nozzle->mFludd->mMario;

    if (nozzle->mFludd->mCurrentNozzle != TWaterGun::Hover)
        return;

    nozzle->mEmitPerFrame = 1.0f;
    nozzle->mEmitRandom = 0.0f;

    if ((nozzle->mMaxSprayQuarterFrames - nozzle->mSprayQuarterFramesLeft) >= 20)
        return;

    if (!player->mControllerWork->mAnalogR < 0.9f || !player->mControllerWork->isFramePressed(TMarioControllerWork::A))
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

static void spamHoverWrapper(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    void (*virtualFunc)(TNozzleTrigger *, u32, TWaterEmitInfo *);
    asm volatile ( "mr %0, r12" : "=r"(virtualFunc));

    checkSpamHover(nozzle, r4, emitInfo);
    virtualFunc(nozzle, r4, emitInfo);
}
kmCall(0x8026C018, &spamHoverWrapper);


static bool checkAirNozzle()
{
    TMario *player;
    asm volatile ( "mr %0, r31" : "=r"(player));
    
    sIsTriggerNozzleDead &= (player->mState & TMario::SA_AIRBORN) != 0;

    TNozzleTrigger *nozzle = (TNozzleTrigger *)getCurrentNozzle__9TWaterGunCFv(player->mFludd);

    if (player->mFludd->mCurrentNozzle == TWaterGun::Spray || player->mFludd->mCurrentNozzle == TWaterGun::Yoshi || player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
    {
        return player->mState != TMario::SA_HOVER_F;
    }

    return (!(player->mState & TMario::SA_AIRBORN) || !sIsTriggerNozzleDead);
}
kmCall(0x80262580, &checkAirNozzle);
kmWrite32(0x80262584, 0x2C030000);