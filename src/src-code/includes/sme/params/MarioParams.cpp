#include "OS.h"
#include "string.h"
#include "sms/actor/Mario.hxx"

#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "params/MarioParams.hxx"

void MarioParams::setPlayer(TMario *player)
{
    this->mPlayer = player;
    this->_setDefaults();
}

bool MarioParams::setFile(MarioParamsFile *data)
{
    if (!data)
    {
        this->mInitialized = false;
        this->_mBaseParams = nullptr;
        Memory::free(this->mParams);
        this->mParams = nullptr;
        return false;
    }

    this->_mBaseParams = data;

    u32 namelen = strlen(this->getPlayerName());
    u32 clslen = Max(sizeof(MarioParamsFile), this->_mBaseParams->Attributes.mNameOffset);
    if (!this->mParams)
    {

        this->mParams = (MarioParamsFile *)Memory::calloc(OSRoundUp32B(clslen + namelen), 4);
    }
    memcpy(this->mParams, this->_mBaseParams, OSRoundUp32B(clslen + namelen));

    this->mInitialized = true;
    return true;
}

const char *MarioParams::getPlayerName() const
{
    if (this->mParams)
    {
        return (const char *)this->mParams + this->mParams->Attributes.mNameOffset;
    }
    else if (this->_mBaseParams)
    {
        return (const char *)this->_mBaseParams + this->_mBaseParams->Attributes.mNameOffset;
    }
    else
    {
        return "Mario";
    }
}

bool MarioParams::canUseNozzle(TWaterGun::NOZZLETYPE nozzle) const
{
    if (this->mParams)
    {
        return this->mParams->Attributes.FluddAttrs.mCanUseNozzle[nozzle];
    }
    else if (this->_mBaseParams)
    {
        return this->_mBaseParams->Attributes.FluddAttrs.mCanUseNozzle[nozzle];
    }
    else
    {
        return true;
    }
}

void MarioParams::_setDefaults()
{
    this->DefaultAttrs.mGravity = this->mPlayer->mGravity;
    this->DefaultAttrs.mTwirlGravity = this->mPlayer->mTwirlGravity;
    this->DefaultAttrs.mDiveStartSpeed = this->mPlayer->mDiveStartSpeed;
    this->DefaultAttrs.mBaseBounce1 = this->mPlayer->mBaseBounceSpeed1;
    this->DefaultAttrs.mBaseBounce2 = this->mPlayer->mBaseBounceSpeed2;
    this->DefaultAttrs.mBaseBounce3 = this->mPlayer->mBaseBounceSpeed3;
    this->DefaultAttrs.mMaxFallNoDamage = this->mPlayer->mMaxFallNoDamage;
    this->DefaultAttrs.mOceanOfs = this->mPlayer->mOceanOfs;
    this->DefaultAttrs.mThrowPower = this->mPlayer->mThrowPower;
    this->DefaultAttrs.mWaterHighBuoyant = this->mPlayer->mWaterHighBuoyant;
    this->DefaultAttrs.mWaterLowBuoyant = this->mPlayer->mWaterLowBuoyant;
    this->DefaultAttrs.mWaterYSpdMultiplier = this->mPlayer->mWaterYSpdMultiplier;
    this->DefaultAttrs.mWaterEntryMaxHeight = this->mPlayer->mWaterEntryMaxHeight;
    this->DefaultAttrs.mWaterMaxDiffToInteract = this->mPlayer->mWaterMaxDiffToInteract;
    this->DefaultAttrs.mWaterJumpHeightDiffMax = this->mPlayer->mWaterJumpHeightDiffMax;
    this->DefaultAttrs.mWaterHealthDrainSpd = this->mPlayer->mWaterHealthDrainSpd;
    this->DefaultAttrs.mWaterHealthScubaDrainSpd = this->mPlayer->mWaterHealthScubaDrainSpd;
    this->DefaultAttrs.mCollisionHeight = 160.0f;
    this->DefaultAttrs.mCollisionWidth = 50.0f;
    this->DefaultAttrs.mMaxGroundSpeed = this->mPlayer->mMaxGroundSpeed;
    this->DefaultAttrs.mHipDropSpeedSlow = this->mPlayer->mHipDropSpeedSlow;
    this->DefaultAttrs.mHipDropSpeedFast = this->mPlayer->mHipDropSpeedFast;
    this->DefaultAttrs.mPoleGrabDistPadding = this->mPlayer->mPoleGrabDistPadding;
    this->DefaultAttrs.mAttackHeight = this->mPlayer->mAttackHeight;
    this->DefaultAttrs.mAttackRadius = this->mPlayer->mAttackRadius;
    this->DefaultAttrs.mReceiveHeight = this->mPlayer->mReceiveHeight;
    this->DefaultAttrs.mReceiveRadius = this->mPlayer->mReceiveRadius;
    this->DefaultAttrs.mPoleClimbSpeedMul = this->mPlayer->mPoleClimbSpeedMul;
    this->DefaultAttrs.mHoverYSpdMultiplier = this->mPlayer->mHoverYSpdMultiplier;
    this->DefaultAttrs.mHoverMaxHeight = 230.0f;
    this->DefaultAttrs.mTurboNozzleSpeed = this->mPlayer->mTurboNozzleSpeed;
    this->DefaultAttrs.mTurboJumpFSpeed = this->mPlayer->mTurboJumpFSpeed;
    this->DefaultAttrs.mTurboJumpYSpeed = this->mPlayer->mTurboJumpYSpeed;
}

void MarioParams::init(TMario *player, bool isMario)
{
    this->mIsEMario = !isMario;
    this->mPlayerID = 0;
    this->mMaxJumps = 1;
    this->mCurJump = 0;
    this->mTerminalVelocity = -75.0f;
    this->mYoshiWaterSpeed = JGeometry::TVec3<float>(0.0f, 0.0f, 0.0f);
    this->mCollisionTimer = 0;
    this->mPrevCollision = 0;
    this->mClimbQuarterFramesLeft = 0;
    this->FluddHistory.mHadFludd = false;
    this->FluddHistory.mMainNozzle = TWaterGun::Spray;
    this->FluddHistory.mSecondNozzle = TWaterGun::Hover;
    this->FluddHistory.mWaterLevel = 0;

    bool hasParams;

    this->setPlayer(player);
    hasParams = this->setFile((MarioParamsFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc("mario"), "/params.bin"));

    if (!hasParams) return;

    if (this->_mBaseParams->Attributes.mMarioHasGlasses)
    {
        wearGlass__6TMarioFv(player);
    }
    this->mClimbQuarterFramesLeft = this->mParams->Attributes.mMaxClimbQuarterFrames;
    this->update();
}

void MarioParams::update()
{
    float sizeMultiplier = this->mParams->Attributes.mSizeMultiplier;
    float scalar = (sizeMultiplier * 0.5f) + (1.0f - 0.5f);

    if (!this->mPlayer->mYoshi || this->mPlayer->mYoshi->mState != TYoshi::MOUNTED)
    {
        this->mPlayer->mSize.x = sizeMultiplier;
        this->mPlayer->mSize.y = sizeMultiplier;
        this->mPlayer->mSize.z = sizeMultiplier;
        this->mPlayer->mModelData->mModel->mSizeMultiplier.x = sizeMultiplier;
        this->mPlayer->mModelData->mModel->mSizeMultiplier.y = sizeMultiplier;
        this->mPlayer->mModelData->mModel->mSizeMultiplier.z = sizeMultiplier;

        this->mParams->Attributes.mBaseJumpHeightMulti = this->_mBaseParams->Attributes.mBaseJumpHeightMulti * scalar;
        this->mParams->Attributes.mSpeedMultiplier = this->_mBaseParams->Attributes.mSpeedMultiplier * scalar;

        this->mPlayer->mHealth = Min(this->mPlayer->mHealth, this->mPlayer->mMaxHealth);

        this->mPlayer->mOBStep = this->mParams->Attributes.mOBStep;
        this->mPlayer->mOBMax = this->mParams->Attributes.mOBMax;
        this->mPlayer->mWallHangTimer = this->mParams->Attributes.mWallHangMax;
        this->mPlayer->mWallAnimTimer = Max(0, this->mParams->Attributes.mWallHangMax - 400);

        this->mPlayer->mGravity = this->DefaultAttrs.mGravity * this->_mBaseParams->Attributes.mGravityMulti * scalar;
        this->mPlayer->mTwirlGravity = this->DefaultAttrs.mTwirlGravity * this->mPlayer->mGravity;
        this->mPlayer->mDiveStartSpeed = this->DefaultAttrs.mDiveStartSpeed * this->mParams->Attributes.mSpeedMultiplier;
        this->mPlayer->mCustomInfo->mTerminalVelocity = -75.0f * this->mPlayer->mGravity;
        this->mPlayer->mMaxFallNoDamage = this->DefaultAttrs.mMaxFallNoDamage * this->_mBaseParams->Attributes.mMaxFallNoDamageMulti * scalar;
        this->mPlayer->mCustomInfo->mMaxJumps = this->mParams->Attributes.mJumpCount;

        this->mPlayer->mWaterHealthDrainSpd = this->DefaultAttrs.mWaterHealthDrainSpd / this->mParams->Attributes.mWaterHealthMultiplier;
        this->mPlayer->mWaterHealthScubaDrainSpd = this->DefaultAttrs.mWaterHealthScubaDrainSpd / this->mParams->Attributes.mWaterHealthMultiplier;
        this->mPlayer->mBaseBounceSpeed1 = this->DefaultAttrs.mBaseBounce1 * this->_mBaseParams->Attributes.mBaseBounce1Multi * scalar;
        this->mPlayer->mBaseBounceSpeed2 = this->DefaultAttrs.mBaseBounce2 * this->_mBaseParams->Attributes.mBaseBounce2Multi * scalar;
        this->mPlayer->mBaseBounceSpeed3 = this->DefaultAttrs.mBaseBounce3 * this->_mBaseParams->Attributes.mBaseBounce3Multi * scalar;
        this->mPlayer->mOceanOfs = this->DefaultAttrs.mOceanOfs * this->mPlayer->mSize.y;
        this->mPlayer->mWaterHighBuoyant = this->DefaultAttrs.mWaterHighBuoyant * scalar;
        this->mPlayer->mWaterLowBuoyant = this->DefaultAttrs.mWaterLowBuoyant * scalar;
        this->mPlayer->mWaterYSpdMultiplier = this->DefaultAttrs.mWaterYSpdMultiplier + ((scalar*0.2f) - 0.2f);
        this->mPlayer->mWaterEntryMaxHeight = this->DefaultAttrs.mWaterEntryMaxHeight * this->mPlayer->mSize.y;
        this->mPlayer->mWaterMaxDiffToInteract = this->DefaultAttrs.mWaterMaxDiffToInteract * this->mPlayer->mSize.y;
        this->mPlayer->mWaterJumpHeightDiffMax = this->DefaultAttrs.mWaterJumpHeightDiffMax * this->mPlayer->mSize.y;
        this->mPlayer->mHipDropSpeedSlow = this->DefaultAttrs.mHipDropSpeedSlow * scalar;
        this->mPlayer->mHipDropSpeedFast = this->DefaultAttrs.mHipDropSpeedFast * scalar;
        this->mPlayer->mThrowPower = this->DefaultAttrs.mThrowPower * this->_mBaseParams->Attributes.mThrowPowerMultiplier * scalar;
        this->mPlayer->mPoleGrabDistPadding = this->DefaultAttrs.mPoleGrabDistPadding * this->mPlayer->mSize.z;
        this->mPlayer->mAttackHeight = this->DefaultAttrs.mAttackHeight * this->mPlayer->mSize.y;
        this->mPlayer->mAttackRadius = this->DefaultAttrs.mAttackRadius * this->mPlayer->mSize.z;
        this->mPlayer->mReceiveHeight = this->DefaultAttrs.mReceiveHeight * this->mPlayer->mSize.y;
        this->mPlayer->mReceiveRadius = this->DefaultAttrs.mReceiveRadius * this->mPlayer->mSize.z;
        this->mPlayer->mPoleClimbSpeedMul = this->DefaultAttrs.mPoleClimbSpeedMul * scalar;

        this->mPlayer->mHoverYSpdMultiplier = this->DefaultAttrs.mHoverYSpdMultiplier * scalar;
        this->mPlayer->mTurboNozzleSpeed = this->DefaultAttrs.mTurboNozzleSpeed * this->mParams->Attributes.mSpeedMultiplier;
        this->mPlayer->mTurboJumpFSpeed = this->DefaultAttrs.mTurboJumpFSpeed * this->mParams->Attributes.mSpeedMultiplier;
        this->mPlayer->mTurboJumpYSpeed = this->DefaultAttrs.mTurboJumpYSpeed * scalar;
        if (this->mPlayer->mFludd)
        {
            this->mPlayer->mFludd->mHoverMaxHeight = this->DefaultAttrs.mHoverMaxHeight * scalar;
        }
    }
}
