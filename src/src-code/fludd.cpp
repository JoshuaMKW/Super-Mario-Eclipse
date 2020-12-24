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

//0x80248F14
bool isPumpOK(TMarioAnimeData *animeData)
{
    return (animeData->mFluddEnabled != animeData->FLUDD_DISABLED && gpMarioAddress->mCustomInfo->mCurJump > 1);
}
kmBranch(0x80248F14, &isPumpOK);

//0x8014206C
bool hasWaterCardOpen()
{
    register TGCConsole2 *gcConsole;
    __asm { mr gcConsole, r31 };

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
bool canCollectFluddItem(TMario *player)
{
    if (player->mCustomInfo->mParams)
        return (onYoshi__6TMarioCFv(player) || !player->mCustomInfo->mParams->Attributes.mCanUseFludd);
    else
        return onYoshi__6TMarioCFv(player);
}
kmCall(0x80283058, &canCollectFluddItem);

//0x8024E710
void sprayGoopMap(TPollutionManager *gpPollutionManager, f32 x, f32 y, f32 z, f32 r)
{
    register TMario *player;
    __asm { mr player, r30 };

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
kmCall(0x8024E710, &sprayGoopMap);

//0x800FED3C
bool canCleanSeals(TWaterManager *gpWaterManager)
{
    MarioParamsFile *localfile = gpMarioAddress->mCustomInfo->mParams;
    return (gpWaterManager->mWaterCardType != 0 || (localfile && localfile->Attributes.FluddAttrs.mCanCleanSeals));
}
kmCall(0x800FED3C, &canCleanSeals);
kmWrite32(0x800FED40, 0x2C030000);

//0x8024D560
void bindFluddtojoint(TWaterGun *gpFludd, void *joint)
{
    MarioParamsFile *localfile = gpFludd->mMario->mCustomInfo->mParams;
    u32 *jointlist;
    u32 index;

    if (localfile)
    {
        index = localfile->Attributes.FluddAttrs.mBindToJointID[(u8)gpFludd->mCurrentNozzle];
        joint = *gpFludd->mMario->mModelData->mModel->mJointArray + index;
    }
    setBaseTRMtx__9TWaterGunFPA4_f(gpFludd, joint);
}
kmCall(0x8024D560, &bindFluddtojoint);
