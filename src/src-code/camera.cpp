#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

//0x80027548
void modifyRangeToSize(f32 *params, f32 *saveParams)
{
    params[0xA8 / 4] = saveParams[0x3B0 / 4];

    //Custom code here
    if (gpMarioAddress->mCustomInfo->mParams)
    {
        if (!gpMarioAddress->mYoshi || gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED || gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier > 1)
        {
            params[0x8 / 4] *= (f32)((f32)(gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5) + (f32)(1 - 0.5));
            params[0xC / 4] *= (f32)((f32)(gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5) + (f32)(1 - 0.5));
            params[0x24 / 4] *= (f32)((f32)(gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.9375) + (f32)(1 - 0.9375));
        }
    }
}