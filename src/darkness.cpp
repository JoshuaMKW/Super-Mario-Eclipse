#include <SMS/Manager/FlagManager.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>

#include "darkness.hxx"
#include "globals.hxx"
#include "settings.hxx"

using namespace BetterSMS;

static TLightContext sLightContext;
static u8 sBrightLevel = 255;

#define MIN_DARKNESS 100
#define MIN_DARKNESS_CORONA 180
#define MAX_DARKNESS 210
#define MIN_SIZE     0.0f
#define MAX_SIZE     500000.0f
#define GROW_BASE    4500.0f
#define GROW_POWER   1.5f

extern DarknessSetting gDarknessSetting;

void initializeParameters(TLightContext::ActiveType type, TVec3f pos, u8 layer_count,
                          JUtility::TColor color, f32 scale, f32 layer_scale, u8 brightness) {
    bool isCoronaMountainBeat  = TFlagManager::smInstance->getBool(0x10077);
    sLightContext.mLightType   = type;
    sLightContext.mTranslation = pos;
    sLightContext.mColor       = color;
    sLightContext.mBaseScale   = scale;
    sBrightLevel               = brightness;

    u8 minDarkness = 0;

    if (type == TLightContext::ActiveType::STATIC) {
        if (gDarknessSetting.getBool()) {
            minDarkness               = isCoronaMountainBeat ? MIN_DARKNESS_CORONA : MIN_DARKNESS;
            sLightContext.mLayerScale = layer_scale;
            sLightContext.mLayerCount = layer_count;
        } else {
            minDarkness = isCoronaMountainBeat ? MIN_DARKNESS_CORONA + 30 : MIN_DARKNESS + 90;
            sLightContext.mLayerScale = 0.1f;
            sLightContext.mLayerCount = 5;
        }
    } else {
        minDarkness               = isCoronaMountainBeat ? MIN_DARKNESS_CORONA : MIN_DARKNESS;
        sLightContext.mLayerScale = layer_scale;
        sLightContext.mLayerCount = layer_count;
    }

    SME::TGlobals::setMinDarkness(minDarkness);
}

void initToDefault(TMarDirector *director) {
    initializeParameters(TLightContext::ActiveType::DISABLED, {0, 0, 0}, 5, {0, 0, 0, 0}, 10000,
                         0.5, 255);
}

void TLightContext::process(TModelWaterManager &manager) {
    u32 shinesCollected       = TFlagManager::smInstance->getFlag(0x40000);
    bool isCoronaMountainBeat = TFlagManager::smInstance->getBool(0x10077);

    switch (mLightType) {
    case TLightContext::ActiveType::STATIC: {
        manager.mLayerCount = isCoronaMountainBeat ? 1 : sLightContext.mLayerCount;

        if (sBrightLevel != 255)
            manager.mDarkLevel = sBrightLevel;
        else if (shinesCollected >= MaxShineCount) {
            if (manager.mDarkLevel < 255)
                manager.mDarkLevel += 1;
            else
                mLightType = TLightContext::ActiveType::DISABLED;
        }

        gShineShadowPos = mTranslation;

        const f32 sigOfs      = 300.0f;
        const f32 sigStrength = 0.04f;

        if (!mSizeMorphing) {
            if (shinesCollected != mPrevShineCount) {
                mPrevSize = manager.mSize;
                mNextSize =
                    mBaseScale +
                    powf(GROW_BASE * ((f32)shinesCollected / (f32)MaxShineCount), GROW_POWER);
                mPrevDarkness = manager.mDarkLevel;
                mNextDarkness =
                    lerp<u8>(SME::TGlobals::getMinDarkness(), MAX_DARKNESS,
                             static_cast<f32>(shinesCollected) / static_cast<f32>(MaxShineCount));

                mPrevShineCount = shinesCollected;
                mSizeMorphing   = true;
                mStepContext    = 0.0f;
                OSReport("PrevSize: %f, NextSize: %f\n", mPrevSize, mNextSize);
            } else {
                break;
            }
        }

        const f32 cur = isCoronaMountainBeat
                            ? MAX_SIZE
                            : sigmoid(mStepContext, mPrevSize, mNextSize, sigOfs, sigStrength);

        if (sBrightLevel == 255)
            manager.mDarkLevel =
                static_cast<u8>(sigmoid(mStepContext, static_cast<f32>(mPrevDarkness),
                                        static_cast<f32>(mNextDarkness), sigOfs, sigStrength));

        constexpr f32 deadZone = 1.0f;

        const bool isFinished = manager.mDarkLevel == mNextDarkness;

        if (manager.mSize > MAX_SIZE) {
            manager.mSize      = MAX_SIZE;
            manager.mDarkLevel = mNextDarkness;
            mSizeMorphing      = false;
        } else if (manager.mSize < MIN_SIZE) {
            manager.mSize      = MIN_SIZE;
            manager.mDarkLevel = mNextDarkness;
            mSizeMorphing      = false;
        } else if (!isFinished) {
            manager.mSize       = cur;
            manager.mSphereStep = cur * mLayerScale;
            mStepContext += 1.0f;
        } else {
            manager.mSize       = cur;
            manager.mSphereStep = cur * mLayerScale;
            mSizeMorphing       = false;
        }
        break;
    }
    case TLightContext::ActiveType::FOLLOWPLAYER: {
        manager.mDarkLevel  = sBrightLevel;
        manager.mSphereStep = mBaseScale * mLayerScale;
        manager.mSize       = mBaseScale;
        manager.mColor      = mColor;
        gShineShadowPos     = gpMarioAddress->mTranslation;
        break;
    }
    case TLightContext::ActiveType::FOLLOWCAMERA: {
        manager.mDarkLevel = sBrightLevel;
        if (((f32 *)gpCamera)[0x134 / 4] > 13000.0f) {
            manager.mDarkLevel = lerp<f32>(
                static_cast<f32>(sBrightLevel), 255.0f,
                clamp(1.0f - ((21000.0f - ((f32 *)gpCamera)[0x134 / 4]) / 8000.0f), 0.0f, 1.0f));
        }
        manager.mSphereStep = mBaseScale * mLayerScale;
        manager.mSize       = mBaseScale;
        manager.mColor      = mColor;
        gShineShadowPos     = gpCamera->mTranslation + mTranslation;
        break;
    }
    default:
        break;
    }

    manager.LightType.mShowShadow = manager.mDarkLevel < 255;
}

static void initShineShadow() {
    if (sLightContext.mLightType == TLightContext::ActiveType::DISABLED)
        return;

    u32 shinesCollected = TFlagManager::smInstance->getFlag(0x40000);
    // bool isCoronaMountainBeat = TFlagManager::smInstance->getBool(0x10077);

    if (shinesCollected < MaxShineCount ||
        sLightContext.mLightType == TLightContext::ActiveType::FOLLOWPLAYER ||
        sLightContext.mLightType == TLightContext::ActiveType::FOLLOWCAMERA) {
        sLightContext.mPrevShineCount = shinesCollected;

        gpModelWaterManager->mColor      = sLightContext.mColor;
        gpModelWaterManager->mDarkLevel  = sBrightLevel;
        gpModelWaterManager->mSphereStep = sLightContext.mBaseScale * sLightContext.mLayerScale;
        gpModelWaterManager->mSize       = sLightContext.mBaseScale;
        gpModelWaterManager->LightType.mMaskObjects = true;
        gpModelWaterManager->LightType.mShowShadow  = true;

        if (sLightContext.mLightType == TLightContext::ActiveType::STATIC) {
            sLightContext.mNextSize =
                sLightContext.mBaseScale +
                powf(GROW_BASE * ((f32)shinesCollected / (f32)MaxShineCount), GROW_POWER);

            if (sBrightLevel == 255)
                gpModelWaterManager->mDarkLevel =
                    lerp<u8>(SME::TGlobals::getMinDarkness(), MAX_DARKNESS,
                             static_cast<f32>(shinesCollected) / static_cast<f32>(MaxShineCount));
            else
                gpModelWaterManager->mDarkLevel = sBrightLevel;

            gpModelWaterManager->mSize = sLightContext.mNextSize;
            gpModelWaterManager->mSphereStep =
                gpModelWaterManager->mSize * sLightContext.mLayerScale;
        }
    } else {
        sLightContext.mLightType = TLightContext::ActiveType::DISABLED;
    }
}
SMS_PATCH_B(SMS_PORT_REGION(0x80280180, 0x80277F0C, 0, 0), initShineShadow);

BETTER_SMS_FOR_CALLBACK void manageShineDarkness(TMarDirector *director) {
    sLightContext.process(*gpModelWaterManager);
}

static bool checkIfActive() {
    return sLightContext.mLightType != TLightContext::ActiveType::DISABLED;
}
SMS_PATCH_BL(0x8027C6A4, checkIfActive);
SMS_WRITE_32(0x8027C6A8, 0x28030001);
