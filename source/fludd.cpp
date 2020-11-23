#include "includes/eclipse.hxx"

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

//0x8014206C
bool hasWaterCardOpen(TGCConsole2 *gcConsole)
{
    //mr r3, r31
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mYoshi->mState != TYoshi::STATE::MOUNTED && gpMario->mCustomInfo->mParams)
    {
        gpMario->mAttributes.mHasFludd = gpMario->mCustomInfo->mParams->Attributes.mCanUseFludd;
    }
    else
    {
        gpMario->mAttributes.mHasFludd = true;
    }

    if (gpMario->mYoshi->mState != TYoshi::STATE::MOUNTED &&
        gpMario->mAttributes.mHasFludd == false &&
        gcConsole->mWaterCardFalling == false &&
        gcConsole->mIsWaterCard == true)
    {

        startDisappearTank__11TGCConsole2Fv(gcConsole);
    }

    return gcConsole->mIsWaterCard;
    //mr r0, r3
}

//0x80283058
bool canCollectFluddItem(TMario *gpMario)
{
    if (gpMario->mCustomInfo->mParams)
        return (onYoshi__6TMarioCFv(gpMario) || !gpMario->mCustomInfo->mParams->Attributes.mCanUseFludd);
    else
        return onYoshi__6TMarioCFv(gpMario);
}

//0x8024E710
/*
__push_stack r30, 0x20, TRUE
mr r4, r30
__call r0, 0x800050E0
__pop_stack r30, 0x20, TRUE
*/

//0x800050E0
void sprayGoopMap(TPollutionManager *gpPollutionManager, TMario *gpMario, float x, float y, float z, float r)
{
    MarioParamsFile *localfile = gpMario->mCustomInfo->mParams;
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

//0x800FED3C
bool canCleanSeals(TWaterManager *gpWaterManager)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    MarioParamsFile *localfile = gpMario->mCustomInfo->mParams;
    return (gpWaterManager->mWaterCardType != 0 || (localfile && localfile->Attributes.FluddAttrs.mCanCleanSeals));
}

//0x8024D560
void bindFluddtojoint(TWaterGun *gpFludd, Matrix3D *joint)
{
    MarioParamsFile *localfile = gpFludd->mMario->mCustomInfo->mParams;
    u32 *jointlist;
    u32 index;

    if (localfile)
    {
        index = localfile->Attributes.FluddAttrs.mBindToJointID[(u8)gpFludd->mCurrentNozzle] * 0x30;
        joint = gpFludd->mMario->mModelData->mModel->mJointArray[index];
    }
    setBaseTRMtx__9TWaterGunFPA4_f(gpFludd, joint);
}

//0x800FED40 -> 2C030000