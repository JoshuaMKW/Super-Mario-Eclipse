#include "includes/eclipse.hxx"

void TMario::setCustomAttributes()
{
    MarioParamsFile *baseParams = this->mCustomInfo->_mBaseParams;
    if (baseParams)
    {
        if (!this->mCustomInfo->mParams)
        {
            this->mCustomInfo->mParams = (MarioParamsFile *)calloc((sizeof(MarioParamsFile) + 63) & -32, 32);
            memcpy(this->mCustomInfo->mParams, baseParams, (sizeof(MarioParamsFile) + 63) & -32);
        }
        MarioParamsFile *params = this->mCustomInfo->mParams;
        float sizeMulti = params->Attributes.mSizeMultiplier;
        float scalar = (float)(sizeMulti * 0.5f) + (float)(1.0f - 0.5f);

        if (this->mCustomInfo->_mFirstParamsDone)
        {
            this->mGravity = this->mCustomInfo->DefaultAttrs.mGravity;
            this->mCustomInfo->mTerminalVelocity = -75.0f * this->mGravity;
            this->mMaxFallNoDamage = this->mCustomInfo->DefaultAttrs.mMaxFallNoDamage;
            this->mCustomInfo->mMaxJumps = 1;

            this->mWaterHealthDrainSpd = this->mCustomInfo->DefaultAttrs.mWaterHealthDrainSpd;
            this->mWaterHealthScubaDrainSpd = this->mCustomInfo->DefaultAttrs.mWaterHealthScubaDrainSpd;
            this->mBaseBounceSpeed1 = this->mCustomInfo->DefaultAttrs.mBaseBounce1;
            this->mBaseBounceSpeed2 = this->mCustomInfo->DefaultAttrs.mBaseBounce2;
            this->mBaseBounceSpeed3 = this->mCustomInfo->DefaultAttrs.mBaseBounce3;
            this->mOceanOfs = this->mCustomInfo->DefaultAttrs.mOceanOfs;
            this->mWaterJumpHeightDifMax = this->mCustomInfo->DefaultAttrs.mWaterJumpHeightDifMax;
            this->mThrowPower = this->mCustomInfo->DefaultAttrs.mThrowPower;

            this->mCustomInfo->mParams->Attributes.mSpeedMultiplier = 1.0f;
            this->mCustomInfo->mParams->Attributes.mBaseJumpHeightMulti = 1.0f;
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
                wearGlass__6TMarioFv(this);
            }

            this->mCustomInfo->DefaultAttrs.mGravity = this->mGravity;
            this->mCustomInfo->DefaultAttrs.mBaseBounce1 = this->mBaseBounceSpeed1;
            this->mCustomInfo->DefaultAttrs.mBaseBounce2 = this->mBaseBounceSpeed2;
            this->mCustomInfo->DefaultAttrs.mBaseBounce3 = this->mBaseBounceSpeed3;
            this->mCustomInfo->DefaultAttrs.mMaxFallNoDamage = this->mMaxFallNoDamage;
            this->mCustomInfo->DefaultAttrs.mOceanOfs = this->mOceanOfs;
            this->mCustomInfo->DefaultAttrs.mThrowPower = this->mThrowPower;
            this->mCustomInfo->DefaultAttrs.mWaterJumpHeightDifMax = this->mWaterJumpHeightDifMax;
            this->mCustomInfo->DefaultAttrs.mWaterHealthDrainSpd = this->mWaterHealthDrainSpd;
            this->mCustomInfo->DefaultAttrs.mWaterHealthScubaDrainSpd = this->mWaterHealthScubaDrainSpd;

            this->mHealth = params->Attributes.mHealth;
            this->mMaxHealth = params->Attributes.mMaxHealth;
            this->mOBStep = params->Attributes.mOBStep;
            this->mOBMax = params->Attributes.mOBMax;
            this->mWallHangTimer = params->Attributes.mWallHangMax;
            this->mWallAnimTimer = max(0, params->Attributes.mWallHangMax - 400);

            this->mAttributes.mGainHelmet = params->Attributes.mMarioHasHelmet;
            this->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
            this->mAttributes.mIsShineShirt = params->Attributes.mMarioHasShirt;
        
            this->mCustomInfo->_mFirstParamsDone = true;
        }
        
        if (!this->mYoshi || this->mYoshi->mState != TYoshi::MOUNTED)
        {
            this->mSize.x *= sizeMulti;
            this->mSize.y *= sizeMulti;
            this->mSize.z *= sizeMulti;
            this->mModelData->mModel->mSizeMultiplier.x *= sizeMulti;
            this->mModelData->mModel->mSizeMultiplier.y *= sizeMulti;
            this->mModelData->mModel->mSizeMultiplier.z *= sizeMulti;

            this->mMaxHealth = params->Attributes.mHealth;
            if (this->mHealth > this->mMaxHealth)
            {
                this->mHealth = this->mMaxHealth;
            }
            this->mOBStep = params->Attributes.mOBStep;
            this->mOBMax = params->Attributes.mOBMax;
            this->mWallHangTimer = params->Attributes.mWallHangMax;
            this->mWallAnimTimer = max(0, params->Attributes.mWallHangMax - 400);

            this->mGravity *= params->Attributes.mGravityMulti;
            this->mCustomInfo->mTerminalVelocity *= params->Attributes.mGravityMulti;
            this->mMaxFallNoDamage *= params->Attributes.mMaxFallNoDamageMulti * scalar;
            this->mCustomInfo->mMaxJumps = params->Attributes.mJumpCount;

            this->mWaterHealthDrainSpd /= params->Attributes.mWaterHealthMultiplier;
            this->mWaterHealthScubaDrainSpd /= params->Attributes.mWaterHealthMultiplier;
            this->mBaseBounceSpeed1 *= params->Attributes.mBaseBounce1Multi * scalar;
            this->mBaseBounceSpeed2 *= params->Attributes.mBaseBounce2Multi * scalar;
            this->mBaseBounceSpeed3 *= params->Attributes.mBaseBounce3Multi * scalar;
            this->mOceanOfs *= this->mSize.y;
            this->mWaterJumpHeightDifMax *= this->mSize.y;
            this->mThrowPower *= params->Attributes.mThrowPowerMultiplier * scalar;

            params->Attributes.mBaseJumpHeightMulti = baseParams->Attributes.mBaseJumpHeightMulti * scalar;
            params->Attributes.mSpeedMultiplier = baseParams->Attributes.mSpeedMultiplier * scalar;
        }
    }
    else
    {
        this->mCustomInfo->mMaxJumps = 1;
    }
}

extern "C" s16 shadowCrashPatch();
kmCall(0x802320E0, &shadowCrashPatch);


//0x8024D2C4
/*
TMario* updateMario(TMario* gpMario) {
    if (gInfo.PlayerData.mCurPlayerID[i] != prevPlayerID) {
        u32* marioVolumeData = (u32*)getVolume__13JKRFileLoaderFPCc("mario");
        u32* marioDataArray = (u32*)*(u32*)ARCBufferMario;

        if (marioDataArray[gInfo.PlayerData.mCurPlayerID[i]]) {
            __dt__13JKRMemArchiveFv(marioVolumeData);
            __ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData, marioDataArray[i], 0, 0);

            initModel__6TMarioFv(gpMario);
            initMirrorModel__6TMarioFv(gpMario);
        }
    }
    asm("lhz 0, 0x114 (3)");
    return gpMario;
}
*/

//0x801E4118
/*
void rescaleHeldObj(float *holderMatrix, float *destMatrix, TMapObjBase *mHeldObj)
{
    asm ("mr 5, 31");
    PSMTXCopy(holderMatrix, destMatrix);

    PSMTXScaleApply(destMatrix, destMatrix, 1 / mHeldObj->mHolder->mSize.x);
}
*/

//0x8024D538
/*
void rescaleHeldObj(TMario *gpMario)
{
    PSMTXScaleApply(getTakingMtx__6TMarioFv(gpMario), getTakingMtx__6TMarioFv(gpMario), 1 / gpMario->mSize.x);
    animSound__6TMarioFv(gpMario);
}
*/

//0x80213314
/*
lis r3, TMario@ha
lwz r3, TMario@l (r3)
lfs f0, 0x24 (r3)
fmuls f30, f30, f0
lwz r3, -0x6220 (r13)
*/

//0x8024E02C
void manageExtraJumps(TMario *gpMario)
{
    if (!isMario__6TMarioFv(gpMario))
        return;

    if ((gpMario->mState & TMario::SA_AIRBORN) == false ||
        (gpMario->mState & 0x800000) ||
        gpMario->mYoshi->mState == TYoshi::MOUNTED)
    {
        gpMario->mCustomInfo->mCurJump = 1;
    }
    else
    {
        if (gpMario->mController->isFramePressed(TMarioGamePad::A) &
            gpMario->mCustomInfo->mCurJump < gpMario->mCustomInfo->mMaxJumps &
            gpMario->mState != TMario::SA_WALLSLIDE)
        {
            if ((gpMario->mCustomInfo->mMaxJumps - gpMario->mCustomInfo->mCurJump) == 1)
            {
                if (gpMario->mState != TMario::SA_TRIPLE_J)
                {
                    changePlayerJumping__6TMarioFUlUl(gpMario, TMario::SA_TRIPLE_J, 0);
                }
                else
                {
                    setStatusToJumping__6TMarioFUlUl(gpMario, TMario::SA_TRIPLE_J, 0);
                }
            }
            else if ((gpMario->mState - TMario::SA_JUMP) > 1)
            {
                changePlayerJumping__6TMarioFUlUl(gpMario, TMario::SA_JUMP, 0);
            }
            else
            {
                changePlayerJumping__6TMarioFUlUl(gpMario, gpMario->mState ^ 1, 0);
            }
            gpMario->mCustomInfo->mCurJump += 1;
        }
    }
    stateMachine__6TMarioFv(gpMario);
}
kmCall(0x8024E02C, &manageExtraJumps);

//0x80254884, 0x8025474C, 0x802546E4, 0x802546B0, 0x80254540 -> 0x800050E8
/*
mflr r0
stw r0, 0x4 (sp)
stwu sp, -0x10 (sp)
stw r31, 0x8 (sp)
mr r31, r4
lis r3, TMario@ha
lwz r3, TMario@l (r3)
cmpw r30, r3
fmadds f0, f2, f1, f0
bne notMario
mr r3, r30
fmr f3, f0
__call r0, 0x80004A78
fmr f0, f1
notMario:
mr r3, r30
mr r4, r31
lwz r31, 0x8 (sp)
addi sp, sp, 0x10
lwz r0, 0x4 (sp)
mtlr r0
blr
*/

//0x80004A78
float calcJumpPower(TMario *gpMario, float factor, float curYVelocity, float jumpPower)
{
    if (gpMario->mCustomInfo->mParams)
    {
        jumpPower *= gpMario->mCustomInfo->mParams->Attributes.mBaseJumpHeightMulti;
        if (gpMario->mState & TMario::SA_AIRBORN)
        {
            jumpPower *= powf(gpMario->mCustomInfo->mParams->Attributes.mMultiJumpMultiplier, (float)gpMario->mCustomInfo->mCurJump);
            gpMario->mForwardSpeed *= gpMario->mCustomInfo->mParams->Attributes.mMultiJumpFSpeedMulti;
        }
    }
    return (curYVelocity * factor) + jumpPower;
}

//0x8025B8C8 - fSpeed Limit Multiplier - ground
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f2, 0x3C (r11)
fmuls f31, f31, f2
notMario:
lfs f2, 0xB0 (r31)
*/

//0x8025B8F8 - fSpeed Multiplier - ground
/*
lfs f1, 0xEF4 (r31)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f0, 0x3C (r11)
fdivs f1, f1, f0
notMario:
*/

//0x8024CC58 - fSpeed Limit Multiplier - jump
/*
lfs f0, -0x1048 (rtoc)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fmuls f0, f0, f5
notMario:
*/

//0x8024CC2C - fSpeed Multiplier - jump
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fmuls f1, f1, f5
notMario:
fmadds f0, f1, f2, f0
*/

//0x80272FF0 - fSpeed Multplier - swim
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fmuls f2, f2, f5
notMario:
fmadds f0, f2, f1, f0
*/

//0x8025C3F4 - fSpeed Multplier - slide
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r30
bne- notMario
lwz r11, 0x3F4 (r30)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fdivs f2, f2, f5
notMario:
fmul f0, f2, f0
*/

//0x8025BC44 - fSpeed Cap Multiplier
/*
lfs f1, 0x5A0 (r31)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f2, 0x3C (r11)
fmuls f1, f1, f2
notMario:
*/

//0x8024AE80 - fSpeed Multplier - hover
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r30
bne- notMario
lwz r11, 0x3F4 (r30)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fmuls f1, f1, f5
notMario:
fmadds f0, f2, f1, f0
*/

//0x802558B4 - fSpeed Multplier - addvelocity
/*
lfs f0, -0xF1C (rtoc)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r3
bne- notMario
lwz r11, 0x3F4 (r3)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f2, 0x3C (r11)
fmuls f1, f1, f2
notMario:
*/

//0x80259368 - fSpeed Multplier - hover
/*
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f5, 0x3C (r11)
fmuls f1, f1, f5
notMario:
fmadds f0, f2, f1, f0
*/

//0x8024C7C4 - fSpeed Wall Bonk Air Cap Multiplier
/*
lfs f0, 0x8E8 (r28)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r28
bne- notMario
lwz r11, 0x3F4 (r28)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f2, 0x3C (r11)
fmuls f0, f0, f2
notMario:
*/

//0x8025B14C - fSpeed Wall Bonk Ground Cap Multiplier
/*
lfs f0, 0x8E8 (r31)
lis r11, TMario@ha
lwz r11, TMario@l (r11)
cmpw r11, r31
bne- notMario
lwz r11, 0x3F4 (r31)
lwz r11, 0 (r11)
cmpwi r11, NULLPTR
beq notMario
lfs f2, 0x3C (r11)
fmuls f0, f0, f2
notMario:
*/

//0x80256670 - terminal velocity stuff
/*
lwz r11, 0x3F4 (r31)
lfs f0, 0x4 (r11)
*/

//0x8025696C - collision height #1
/*
lfs f1, -0xEDC (rtoc)
lfs f0, 0x28 (r22)
fmuls f1, f0, f1
*/

//0x80256D14 - collision height #2
/*
lfs f2, -0xEDC (rtoc)
lfs f0, 0x28 (r29)
fmuls f2, f0, f2
*/

//0x802573FC - collision height #3
/*
lfs f0, -0xEDC (rtoc)
lfs f2, 0x28 (r30)
fmuls f0, f2, f0
*/

//0x802505F4 - collision width
/*
lfs f1, 0x24 (r29)
lfs f3, 0x2C (r29)
fcmpo cr7, f1, f3
bgt cr7 0x8

fmr f1, f3

fmuls f0, f1, f0
stfs f0, 0x15C (r29)
*/

//0x80256CE8 - grab length multiplier
/*
lfs f2, -0xED8 (rtoc)
lfs f0, 0x28 (r29)
fmuls f2, f2, f0
*/