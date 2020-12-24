#include "OS.h"
#include "sms/actor/Mario.hxx"

#include "sme/params/MarioParams.hxx"
#include "SME.hxx"

void MarioParamsFile::initializeMario(TMario *player)
{
    if (this)
    {
        if (!player->mCustomInfo->mParams)
        {
            player->mCustomInfo->mParams = (MarioParamsFile *)Memory::calloc(OSRoundUp32B(sizeof(MarioParamsFile)), 32);
            memcpy(player->mCustomInfo->mParams, this, (OSRoundUp32B(sizeof(MarioParamsFile))));
        }
        MarioParamsFile *params = player->mCustomInfo->mParams;
        float sizeMulti = params->Attributes.mSizeMultiplier;
        float scalar = (float)(sizeMulti * 0.5f) + (float)(1.0f - 0.5f);

        if (player->mCustomInfo->_mFirstParamsDone)
        {
            player->mGravity = player->mCustomInfo->DefaultAttrs.mGravity;
            player->mCustomInfo->mTerminalVelocity = -75.0f * player->mGravity;
            player->mMaxFallNoDamage = player->mCustomInfo->DefaultAttrs.mMaxFallNoDamage;
            player->mCustomInfo->mMaxJumps = 1;

            player->mWaterHealthDrainSpd = player->mCustomInfo->DefaultAttrs.mWaterHealthDrainSpd;
            player->mWaterHealthScubaDrainSpd = player->mCustomInfo->DefaultAttrs.mWaterHealthScubaDrainSpd;
            player->mBaseBounceSpeed1 = player->mCustomInfo->DefaultAttrs.mBaseBounce1;
            player->mBaseBounceSpeed2 = player->mCustomInfo->DefaultAttrs.mBaseBounce2;
            player->mBaseBounceSpeed3 = player->mCustomInfo->DefaultAttrs.mBaseBounce3;
            player->mOceanOfs = player->mCustomInfo->DefaultAttrs.mOceanOfs;
            player->mWaterJumpHeightDifMax = player->mCustomInfo->DefaultAttrs.mWaterJumpHeightDifMax;
            player->mThrowPower = player->mCustomInfo->DefaultAttrs.mThrowPower;

            player->mCustomInfo->mParams->Attributes.mSpeedMultiplier = 1.0f;
            player->mCustomInfo->mParams->Attributes.mBaseJumpHeightMulti = 1.0f;
        }
        else
        {
            params->Attributes.mBaseBounce1Multi *= scalar;
            params->Attributes.mBaseBounce2Multi *= scalar;
            params->Attributes.mBaseBounce3Multi *= scalar;
            params->Attributes.mMaxFallNoDamageMulti *= scalar;
            params->Attributes.mBaseJumpHeightMulti *= scalar;
            params->Attributes.mSpeedMultiplier *= scalar;
            params->Attributes.mThrowPowerMultiplier *= scalar;

            if (params->Attributes.mMarioHasGlasses)
            {
                wearGlass__6TMarioFv(player);
            }

            player->mCustomInfo->DefaultAttrs.mGravity = player->mGravity;
            player->mCustomInfo->DefaultAttrs.mBaseBounce1 = player->mBaseBounceSpeed1;
            player->mCustomInfo->DefaultAttrs.mBaseBounce2 = player->mBaseBounceSpeed2;
            player->mCustomInfo->DefaultAttrs.mBaseBounce3 = player->mBaseBounceSpeed3;
            player->mCustomInfo->DefaultAttrs.mMaxFallNoDamage = player->mMaxFallNoDamage;
            player->mCustomInfo->DefaultAttrs.mOceanOfs = player->mOceanOfs;
            player->mCustomInfo->DefaultAttrs.mThrowPower = player->mThrowPower;
            player->mCustomInfo->DefaultAttrs.mWaterJumpHeightDifMax = player->mWaterJumpHeightDifMax;
            player->mCustomInfo->DefaultAttrs.mWaterHealthDrainSpd = player->mWaterHealthDrainSpd;
            player->mCustomInfo->DefaultAttrs.mWaterHealthScubaDrainSpd = player->mWaterHealthScubaDrainSpd;

            player->mHealth = params->Attributes.mHealth;
            player->mMaxHealth = params->Attributes.mMaxHealth;
            player->mOBStep = params->Attributes.mOBStep;
            player->mOBMax = params->Attributes.mOBMax;
            player->mWallHangTimer = params->Attributes.mWallHangMax;
            player->mWallAnimTimer = Max(0, params->Attributes.mWallHangMax - 400);

            player->mAttributes.mGainHelmet = params->Attributes.mMarioHasHelmet;
            player->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
            player->mAttributes.mIsShineShirt = params->Attributes.mMarioHasShirt;
        
            player->mCustomInfo->_mFirstParamsDone = true;
        }
        
        if (!player->mYoshi || player->mYoshi->mState != TYoshi::MOUNTED)
        {
            player->mSize.x *= sizeMulti;
            player->mSize.y *= sizeMulti;
            player->mSize.z *= sizeMulti;
            player->mModelData->mModel->mSizeMultiplier.x *= sizeMulti;
            player->mModelData->mModel->mSizeMultiplier.y *= sizeMulti;
            player->mModelData->mModel->mSizeMultiplier.z *= sizeMulti;

            player->mMaxHealth = params->Attributes.mHealth;
            if (player->mHealth > player->mMaxHealth)
            {
                player->mHealth = player->mMaxHealth;
            }
            player->mOBStep = params->Attributes.mOBStep;
            player->mOBMax = params->Attributes.mOBMax;
            player->mWallHangTimer = params->Attributes.mWallHangMax;
            player->mWallAnimTimer = Max(0, params->Attributes.mWallHangMax - 400);

            player->mGravity *= params->Attributes.mGravityMulti;
            player->mCustomInfo->mTerminalVelocity *= params->Attributes.mGravityMulti;
            player->mMaxFallNoDamage *= params->Attributes.mMaxFallNoDamageMulti * scalar;
            player->mCustomInfo->mMaxJumps = params->Attributes.mJumpCount;

            player->mWaterHealthDrainSpd /= params->Attributes.mWaterHealthMultiplier;
            player->mWaterHealthScubaDrainSpd /= params->Attributes.mWaterHealthMultiplier;
            player->mBaseBounceSpeed1 *= params->Attributes.mBaseBounce1Multi * scalar;
            player->mBaseBounceSpeed2 *= params->Attributes.mBaseBounce2Multi * scalar;
            player->mBaseBounceSpeed3 *= params->Attributes.mBaseBounce3Multi * scalar;
            player->mOceanOfs *= player->mSize.y;
            player->mWaterJumpHeightDifMax *= player->mSize.y;
            player->mThrowPower *= params->Attributes.mThrowPowerMultiplier * scalar;

            params->Attributes.mBaseJumpHeightMulti = this->Attributes.mBaseJumpHeightMulti * scalar;
            params->Attributes.mSpeedMultiplier = this->Attributes.mSpeedMultiplier * scalar;
        }
    }
    else
    {
        player->mCustomInfo->mMaxJumps = 1;
    }
}