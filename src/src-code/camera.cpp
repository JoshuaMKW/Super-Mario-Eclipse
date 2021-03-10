#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

// 0x80027548
// extern -> SME.cpp
void modifyCameraRangeToSize(f32 *params, f32 *saveParams)
{
    params[0xA8 / 4] = saveParams[0x3B0 / 4];

    //Custom code here
    if (gpMarioAddress->mCustomInfo->mParams)
    {
        if (!gpMarioAddress->mYoshi || gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED || gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier > 1.0f)
        {
            params[0x8 / 4] *= (f32)((gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5f) + (1.0f - 0.5f));
            params[0xC / 4] *= (f32)((gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.5f) + (1.0f - 0.5f));
            params[0x24 / 4] *= (f32)((gpMarioAddress->mCustomInfo->mParams->Attributes.mSizeMultiplier * 0.9375f) + (1.0f - 0.9375f));
        }
    }
}