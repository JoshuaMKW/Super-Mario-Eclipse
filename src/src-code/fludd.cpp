#include "sms/actor/Mario.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "SME.hxx"

using namespace SME;

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
bool Patch::Fludd::isPumpOK(TMarioAnimeData *animeData)
{
    return (animeData->mFluddEnabled != TMarioAnimeData::FLUDD::FLUDD_DISABLED &&
            SME::TGlobals::sGlobals.getPlayerParams(gpMarioAddress)->mCurJump <= 1);
}

// 0x8014206C
// extern -> SME.cpp
bool Patch::Fludd::hasWaterCardOpen()
{
    TGCConsole2 *gcConsole;
    SME_FROM_GPR(31, gcConsole);

    const SME::Class::TPlayerParams *playerParams = SME::TGlobals::sGlobals.getPlayerParams(gpMarioAddress);

    if (gpMarioAddress->mYoshi->mState != TYoshi::State::MOUNTED)
        gpMarioAddress->mAttributes.mHasFludd =
            playerParams->getParams()->mCanUseFludd.get();
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
bool Patch::Fludd::canCollectFluddItem(TMario *player)
{
    return onYoshi__6TMarioCFv(player) ||
            !SME::TGlobals::sGlobals.getPlayerParams(player)->getParams()->mCanUseFludd.get();
}

static void sprayGoopMap(TPollutionManager *gpPollutionManager, TMario *player,
                         f32 x, f32 y, f32 z, f32 r)
{
    const SME::Class::TPlayerParams *playerParams = SME::TGlobals::sGlobals.getPlayerParams(player);
    const SME::Class::TCustomParams *prm = playerParams->getParams();

    if (!playerParams->isMario() || !playerParams->isInitialized())
        clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);

    if (prm->mCleaningType.get() != SME::Class::TCustomParams::FluddCleanType::NONE)
    {
        if (prm->mCleaningType.get() == SME::Class::TCustomParams::FluddCleanType::CLEAN)
            clean__17TPollutionManagerFffff(gpPollutionManager, x, y, z, r);
        else if (prm->mCleaningType.get() == SME::Class::TCustomParams::FluddCleanType::GOOP)
            stamp__17TPollutionManagerFUsffff(gpPollutionManager, 1, x, y, z, r);
    }
}

// 0x800678C4, 0x801A3ED0, 0x801B42D8, 0x8027F7DC, 0x8027F94C,
// extern -> SME.cpp
void Patch::Fludd::sprayGoopMapWrapGlobalMar(TPollutionManager *gpPollutionManager,
                               f32 x, f32 y, f32 z, f32 r)
{
    sprayGoopMap(gpPollutionManager, gpMarioAddress, x, y, z, r);
}

// 0x8024E710
// extern -> SME.cpp
void Patch::Fludd::sprayGoopMapWrapMar30(TPollutionManager *gpPollutionManager,
                           f32 x, f32 y, f32 z, f32 r)
{
    TMario *player;
    SME_FROM_GPR(30, player);

    sprayGoopMap(gpPollutionManager, player, x, y, z, r);
}

// 0x800FED3C
// extern -> SME.cpp
bool Patch::Fludd::canCleanSeals(TWaterManager *gpWaterManager)
{
    return gpWaterManager->mWaterCardType != 0 ||
           SME::TGlobals::sGlobals.getPlayerParams(gpMarioAddress)->getParams()->mCanCleanSeals.get();
}

// 0x8024D53C
// extern -> SME.cpp
TWaterGun *Patch::Fludd::bindFluddtojoint()
{
    TMario *player;
    SME_FROM_GPR(31, player);

    player->mBindBoneIDArray[0] =
        SME::TGlobals::sGlobals.getPlayerParams(gpMarioAddress)->getNozzleBoneID(static_cast<TWaterGun::NozzleType>(player->mFludd->mCurrentNozzle));

    return player->mFludd;
}

// 0x80262580
// extern -> SME.cpp
void Patch::Fludd::checkExecWaterGun(TWaterGun *fludd)
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
void Patch::Fludd::killTriggerNozzle()
{
    TNozzleTrigger *nozzle;
    SME_FROM_GPR(29, nozzle);

    nozzle->mSprayState = TNozzleTrigger::DEAD;
    if (nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Hover ||
        nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Rocket ||
        nozzle->mFludd->mCurrentNozzle == TWaterGun::NozzleType::Turbo)
        sIsTriggerNozzleDead = true;
    
}

static void checkSpamHover(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    TMario *player = nozzle->mFludd->mMario;
    Vec size;
    player->JSGGetScaling(&size);

    if (nozzle->mFludd->mCurrentNozzle != TWaterGun::Hover)
        return;

    nozzle->mEmitPerFrame = 1.0f;
    nozzle->mEmitRandom = 0.0f;

    if ((nozzle->mMaxSprayQuarterFrames - nozzle->mSprayQuarterFramesLeft) >= 20)
        return;

    if (!(player->mControllerWork->mAnalogR < 0.9f) ||
        !(player->mControllerWork->mFrameInput & TMarioControllerWork::A))
        return;

    if (nozzle->mFludd->mCurrentWater < 510)
        return;

    nozzle->mEmitPerFrame = 255.0f;
    nozzle->mEmitRandom = 0.5f;
    nozzle->mSprayQuarterFramesLeft = 0;
    nozzle->mSprayState = TNozzleTrigger::DEAD;

    nozzle->mFludd->mCurrentWater -= 255;
    player->mSpeed.y += (70.0f * size.y) - player->mSpeed.y;
    player->mJumpingState &= 0xFFFFFEFF;

    sIsTriggerNozzleDead = true;
    return;
}

// 0x80262580
// extern -> SME.cpp
void Patch::Fludd::spamHoverWrapper(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo)
{
    void (*virtualFunc)(TNozzleTrigger *, u32, TWaterEmitInfo *);
    SME_FROM_GPR(12, virtualFunc);

    checkSpamHover(nozzle, r4, emitInfo);
    virtualFunc(nozzle, r4, emitInfo);
}

// 0x80262580
// extern -> SME.cpp
bool Patch::Fludd::checkAirNozzle()
{
    TMario *player;
    SME_FROM_GPR(31, player);
    
    sIsTriggerNozzleDead &= (player->mState & static_cast<u32>(TMario::State::AIRBORN)) != 0;

    if (player->mFludd->mCurrentNozzle == TWaterGun::Spray || player->mFludd->mCurrentNozzle == TWaterGun::Yoshi || player->mFludd->mCurrentNozzle == TWaterGun::Underwater)
        return player->mState != static_cast<u32>(TMario::State::HOVER_F);

    return (!(player->mState & static_cast<u32>(TMario::State::AIRBORN)) || !sIsTriggerNozzleDead);
}