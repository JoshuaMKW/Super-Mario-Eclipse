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

void initializeParameters(TLightContext::ActiveType type, TVec3f pos, u8 layer_count,
                          JUtility::TColor color, f32 scale, f32 layer_scale, u8 brightness) {
    sLightContext.mLightType   = type;
    sLightContext.mTranslation = pos;
    sLightContext.mColor       = color;
    sLightContext.mBaseScale   = scale;
    sLightContext.mLayerScale  = layer_scale;
    sLightContext.mLayerCount  = layer_count;
    sBrightLevel               = brightness;
}

void initToDefault(TMarDirector *director) {
    initializeParameters(TLightContext::ActiveType::DISABLED, {0, 0, 0}, 5, {0, 0, 0, 0}, 10000,
                         0.5, 255);
}

void TLightContext::process(TModelWaterManager &manager) {
    s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;

    manager.mLayerCount = sLightContext.mLayerCount;

    switch (mLightType) {
    case TLightContext::ActiveType::STATIC: {
        if (sBrightLevel != 255)
            manager.mDarkLevel = sBrightLevel;
        else if (CurrentShineCount >= MaxShineCount) {
            if (manager.mDarkLevel < 255)
                manager.mDarkLevel += 1;
            else
                mLightType = TLightContext::ActiveType::DISABLED;
        }

        gShineShadowPos = mTranslation;

        const f32 sigOfs      = 300.0f;
        const f32 sigStrength = 0.04f;

        if (!mSizeMorphing) {
            if (CurrentShineCount != mPrevShineCount) {
                mPrevSize = manager.mSize;
                mNextSize =
                    mBaseScale + powf(((1350.0f / MaxShineCount) * CurrentShineCount), 1.5f);
                mPrevDarkness = manager.mDarkLevel;
                mNextDarkness =
                    lerp<u8>(SME::TGlobals::getMinDarkness(), 190,
                             static_cast<f32>(CurrentShineCount) / static_cast<f32>(MaxShineCount));

                mSizeMorphing = true;
                mStepContext  = 0.0f;
            } else {
                break;
            }
        }

        const f32 cur = sigmoid(mStepContext, mPrevSize, mNextSize, sigOfs, sigStrength);

        if (sBrightLevel == 255)
            manager.mDarkLevel =
                static_cast<u8>(sigmoid(mStepContext, static_cast<f32>(mPrevDarkness),
                                        static_cast<f32>(mNextDarkness), sigOfs, sigStrength));

        constexpr f32 deadZone = 1.0f;
        constexpr f32 minSize  = 0.0f;
        constexpr f32 maxSize  = 80000.0f;

        f32 &NextSize = mNextSize;
        f32 &PrevSize = mPrevSize;

        const bool isFinished = NextSize >= PrevSize ? cur >= (NextSize - deadZone)
                                                     : cur <= (NextSize + deadZone);
        if (manager.mSize > maxSize) {
            manager.mSize = maxSize;
            mSizeMorphing = false;
        } else if (manager.mSize < minSize) {
            manager.mSize = minSize;
            mSizeMorphing = false;
        } else if (!isFinished) {
            manager.mSize       = cur;
            manager.mSphereStep = cur * mLayerScale;
            mStepContext += 1.0f;
        } else {
            manager.mSize       = cur;
            manager.mSphereStep = cur * mLayerScale;
            mPrevShineCount     = CurrentShineCount;
            mSizeMorphing       = false;
        }
        break;
    }
    case TLightContext::ActiveType::FOLLOWPLAYER: {
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

    s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
    if (CurrentShineCount < MaxShineCount ||
        sLightContext.mLightType == TLightContext::ActiveType::FOLLOWPLAYER) {
        sLightContext.mPrevShineCount = CurrentShineCount;

        gpModelWaterManager->mColor      = sLightContext.mColor;
        gpModelWaterManager->mDarkLevel  = sBrightLevel;
        gpModelWaterManager->mSphereStep = sLightContext.mBaseScale * sLightContext.mLayerScale;
        gpModelWaterManager->mSize       = sLightContext.mBaseScale;
        gpModelWaterManager->LightType.mMaskObjects = true;
        gpModelWaterManager->LightType.mShowShadow  = true;

        if (sLightContext.mLightType == TLightContext::ActiveType::STATIC) {
            sLightContext.mNextSize = sLightContext.mBaseScale +
                                      powf(((1350.0f / MaxShineCount) * CurrentShineCount), 1.5f);

            if (sBrightLevel == 255)
                gpModelWaterManager->mDarkLevel =
                    lerp<u8>(SME::TGlobals::getMinDarkness(), 190,
                             static_cast<f32>(CurrentShineCount) / static_cast<f32>(MaxShineCount));
            else
                gpModelWaterManager->mDarkLevel = sBrightLevel;

            gpModelWaterManager->mSize       = sLightContext.mNextSize;
            gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
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

//// 0x8024D3A8
//// 0x8003F8F0
//// extern -> SME.cpp
// void Patch::CKit::realTimeCustomAttrsHandler(TMario *player) {
//   if (player->mYoshi)
//     maintainYoshi(player->mYoshi);
//
//   Class::TPlayerData *playerParams = TGlobals::getPlayerData(player);
//   Class::TStageParams *stageParams = Class::TStageParams::sStageConfig;
//
//   const Class::TPlayerParams *params = playerParams->getParams();
//   const f32 sizeMultiplier =
//       params->mSizeMultiplier.get() * stageParams->mPlayerSizeMultiplier.get();
//   const f32 speedMultiplier = params->mSpeedMultiplier.get();
//
//   playerParams->scalePlayerAttrs(sizeMultiplier);
//
// #define SCALE_PARAM(param, scale) param.set(param.get() * scale)
//
//   switch (playerParams->getPlayerID()) {
//   case Enum::Player::MARIO:
//   case Enum::Player::LUIGI:
//   case Enum::Player::IL_PIANTISSIMO:
//   case Enum::Player::SHADOW_MARIO:
//     break;
//   case Enum::Player::DRY_BONES:
//     SCALE_PARAM(player->mRunParams.mMotBlendRunSp, 100.0f);
//     SCALE_PARAM(player->mSwimParams.mWaitBouyancy, -1.0f);
//     SCALE_PARAM(player->mSwimParams.mMoveBouyancy, 0.0f);
//     SCALE_PARAM(player->mSwimParams.mPaddleDown, 2.0f);
//     SCALE_PARAM(player->mSwimParams.mPaddleSpeedUp, 0.5f);
//     break;
//   default:
//     break;
//   }
//
//   SME_EVAL_LOG(playerParams->mIsOnFire);
//   if (playerParams->mIsOnFire) {
//     SCALE_PARAM(player->mDeParams.mRunningMax, 1.4f);
//     SCALE_PARAM(player->mDeParams.mDashMax, 1.4f);
//     SCALE_PARAM(player->mDeParams.mClashSpeed, 1.4f);
//     SCALE_PARAM(player->mJumpParams.mJumpSpeedAccelControl, 1.4f);
//     SCALE_PARAM(player->mJumpParams.mFenceSpeed, 1.4f);
//     SCALE_PARAM(player->mRunParams.mMaxSpeed, 1.4f);
//     SCALE_PARAM(player->mRunParams.mAddBase, 1.4f);
//     SCALE_PARAM(player->mRunParams.mDecBrake, 1.05f);
//     SCALE_PARAM(player->mRunParams.mRunAnmSpeedBase, 1.4f);
//     SCALE_PARAM(player->mHangFenceParams.mMoveSp, 1.4f);
//     SCALE_PARAM(player->mHangFenceParams.mDescentSp, 1.4f);
//     SCALE_PARAM(player->mDirtyParams.mSlipRunSp, 1.4f);
//     SCALE_PARAM(player->mDirtyParams.mSlipCatchSp, 1.4f);
//
//     if (player->mAttributes.mIsShallowWater || player->mAttributes.mIsWater)
//       Util::Mario::extinguishPlayer(player, false);
//     else
//       blazePlayer(player);
//   }
//
// #undef SCALE_PARAM
//
//   setPositions__6TMarioFv(player);
// }