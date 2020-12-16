#include "includes/eclipse.hxx"

//0x80027548
void modifyRangeToSize(float *params, float *saveParams)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    params[0xA8 / 4] = saveParams[0x3B0 / 4];

    //Custom code here
    if (gpMario->mCustomInfo->mParams)
    {
        if (!gpMario->mYoshi || gpMario->mYoshi->mState != TYoshi::MOUNTED || gpMario->mCustomInfo->mParams->Attributes.mSizeMultiplier > 1)
        {
            params[0x8 / 4] *= (float)((float)(gpMario->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5) + (float)(1 - 0.5));
            params[0xC / 4] *= (float)((float)(gpMario->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5) + (float)(1 - 0.5));
            params[0x24 / 4] *= (float)((float)(gpMario->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.9375) + (float)(1 - 0.9375));
        }
    }
}