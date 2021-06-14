#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

using namespace SME;

// 0x80027548
// extern -> SME.cpp
void Patch::Camera::modifyCameraRangeToSize(f32 *params, f32 *saveParams)
{
    params[0xA8 / 4] = saveParams[0x3B0 / 4];

    //Custom code here
    const SME::Class::TPlayerData *playerParams = SME::TGlobals::getPlayerParams(gpMarioAddress);
    if (!playerParams)
        return;
        
    const f32 scale = playerParams->getParams()->mSizeMultiplier.get();

    if (!gpMarioAddress->mYoshi || gpMarioAddress->mYoshi->mState != TYoshi::MOUNTED || scale > 1.0f)
    {
        params[0x8 / 4] *= (scale * 0.5f) + (1.0f - 0.5f);
        params[0xC / 4] *= (scale * 0.5f) + (1.0f - 0.5f);
        params[0x24 / 4] *= (scale * 0.9375f) + (1.0f - 0.9375f);
    }
}