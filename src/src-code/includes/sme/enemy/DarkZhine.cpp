#include "DarkZhine.hxx"

#include "SME.hxx"

/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

TDarkZhine::TDarkZhine(const char *name, bool hardMode) : TBossGesso(name)
{
    ZhineFile *gessoZhineFile = (ZhineFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc("scene"), "/zhine/config.bin");
    this->mHardMode = hardMode;

    *(u32 *)0x803B2A94 = reinterpret_cast<u32>(&TDarkZhine::cleanFromSpineBase);  //Replace eye damage vtable entry
    if (this->mHardMode)
    {
        this->setBoundingPoint(gessoZhineFile->ZhineBinData.mBoundingPoint);
        this->setBoundingRadius(gessoZhineFile->ZhineBinData.mBoundingAreaRadius);
        this->setShockRadius(2000.0f);
        this->setStampRadius(1000.0f);
        this->setMarioDistance(2000.0f);
        this->setSpeedMultiplier(1.2f);
        this->setAccelerationRate(0.1f);
        this->setMaxSpeed(20.0f);
        this->setFramesToCleanOnce(4);
        this->setPoundingTimer(300);
        this->setShockingTimer(120);
        this->setRollingTimer(300);
        this->setGoopingTimer(0);
        this->setPoundingTimerMax(300);
        this->setShockingTimerMax(120);
        this->setRollingTimerMax(300);
        this->setGoopingTimerMax(0);
        this->setRisingRate(1.4f);
        this->setMaxPoundingHeight(800.0f);

        *(f32 *)0x8041014C = gessoZhineFile->ZhineBinData.mBoundingAreaRadius;
        *(f32 *)0x80410150 *= 1.2f;
    }
    else
    {
        this->setBoundingPoint(gessoZhineFile->ZhineBinData.mBoundingPoint);
        this->setBoundingRadius(gessoZhineFile->ZhineBinData.mBoundingAreaRadius);
        this->setShockRadius(2000.0f);
        this->setStampRadius(1000.0f);
        this->setMarioDistance(2000.0f);
        this->setSpeedMultiplier(1.0f);
        this->setAccelerationRate(0.1f);
        this->setMaxSpeed(18.0f);
        this->setFramesToCleanOnce(3);
        this->setPoundingTimer(300);
        this->setShockingTimer(120);
        this->setRollingTimer(300);
        this->setGoopingTimer(0);
        this->setPoundingTimerMax(300);
        this->setShockingTimerMax(120);
        this->setRollingTimerMax(300);
        this->setGoopingTimerMax(0);
        this->setRisingRate(1.4f);
        this->setMaxPoundingHeight(800.0f);

        *(f32 *)0x8041014C = gessoZhineFile->ZhineBinData.mBoundingAreaRadius;
        *(f32 *)0x80410150 *= 1.0f;
    }
    this->mGoopLevel = 0xFF;
    OSReport("Created Zhine boss at %p; Hard mode = %s", this, this->mHardMode ? "True" : "False");
}

TDarkZhine::~TDarkZhine()
{
    *(u32 *)0x803B2A94 = 0x80074E54; //restore eye damage vtable entry
    *(f32 *)0x8041014C = 800.0f;
    *(f32 *)0x80410150 = 3000.0f;
}

f32 TDarkZhine::getAngleToTarget()
{
    JGeometry::TVec3<f32> zhineCoordinates = this->mPosition;

    return atan2f(this->mTarget->mPosition.x - zhineCoordinates.x,
                    this->mTarget->mPosition.z - zhineCoordinates.z);
}

bool TDarkZhine::isTargetInRangeToHome(JGeometry::TVec3<f32> home, f32 r)
{
    return (fabsf(this->mTarget->mPosition.x - home.x) +
            fabsf(this->mTarget->mPosition.y - home.y) +
            fabsf(this->mTarget->mPosition.z - home.z) <
            r);
}

void TDarkZhine::advanceRollMovement(TPollutionManager *gpPollution)
{

    if (this->rollingTimer() < this->rollingTimerMax()/2)
        this->mGroundSpeed = sigmoidCurve(this->rollingTimer(),
                                            0.0f,
                                            this->maxSpeed() * this->speedMultiplier(),
                                            300.0f,
                                            0.04f * this->accelerationRate());
    else
        this->mGroundSpeed = sigmoidCurve(this->rollingTimer() - this->rollingTimerMax()/2, 0, this->maxSpeed()*this->speedMultiplier(), 300.0f, -0.04f*this->accelerationRate());

    if (this->mGroundSpeed > this->maxSpeed())
        this->mGroundSpeed = this->maxSpeed();
    else if (this->mGroundSpeed < 0.0f)
        this->mGroundSpeed = 0.0f;

    this->mPosition.x += (this->mGroundSpeed * sinf(AngleToRadians(this->mRotation.y)));
    this->mPosition.z += (this->mGroundSpeed * cosf(AngleToRadians(this->mRotation.y)));
    this->mSpeed.x = (this->mGroundSpeed * sinf(AngleToRadians(this->mRotation.y)));
    this->mSpeed.z = (this->mGroundSpeed * cosf(AngleToRadians(this->mRotation.y)));

    stamp__17TPollutionManagerFUsffff(gpPollution, 1,
                                        this->mPosition.x,
                                        this->mPosition.y,
                                        this->mPosition.z,
                                        400.0f);
}

TDarkZhine::PoundingState TDarkZhine::advanceDropAttack(TPollutionManager *gpPollution, TMario *player)
{

    if (this->poundingStatus() == TDarkZhine::DROPPING)
    {
        this->mGravity = 1.0f;
        this->mSpeed.x = 0.0f;
        this->mSpeed.z = 0.0f;
        this->mSpeed.y *= this->mSpeedMultiplier;

        if (this->mPosition.y - this->mFloorBelow->mMaxHeight < 10.0f &&
            this->mPosition.y >= this->mFloorBelow->mMinHeight)
        {

            startShake__12TCameraShakeF16EnumCamShakeModef(gpCameraShake, 0x27, 1.0f);
            startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(6158, this->mPosition,
                                                                                    0, 0, 0, 4);
            start__9RumbleMgrFiP3Vec(gpPad1RumbleMgr, 8, this->mPosition);
            stamp__17TPollutionManagerFUsffff(gpPollution, 1,
                                                this->mPosition.x,
                                                this->mFloorBelow->mMaxHeight,
                                                this->mPosition.z,
                                                mStampRadius);

            if (this->mGoopLevel > (0xFF - 0x30))
            {
                this->mGoopLevel = 0xFF;
            }
            else
            {
                this->mGoopLevel += 0x20;
            }

            this->setPoundingStatus(TDarkZhine::SHOCKING);
            this->setStatusTimer(this->shockingTimerMax());
        }
    }
    else if (this->poundingStatus() == TDarkZhine::SHOCKING)
    {
        if (!(player->mState & TMario::SA_AIRBORN) &&
            this->isTargetInRangeToHome(this->mPosition, this->shockRadius()) &&
            (player->mState != TMario::SA_KNCK_LND &&
                player->mState != 0x4045C))
        {
            decHP__6TMarioFi(player, 1);
            changePlayerStatus__6TMarioFUlUlb(player, TMario::SA_KNCK_LND, 0, 0);
        }

        if (this->statusTimer() <= 0)
        {
            this->setPoundingStatus(TDarkZhine::GROUNDROLL);
            this->setStatusTimer(this->rollingTimerMax());
        }
    }
    else if (this->poundingStatus() == TDarkZhine::GROUNDROLL)
    {
        if (this->mPosition.y - this->mFloorBelow->mMaxHeight > 100)
        {
            this->setPoundingStatus(TDarkZhine::RISING);
            this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
        }
        else if (this->rollingTimer() > 0)
        {
            this->advanceRollMovement(gpPollution);
            if (!(this->statusTimer() % 15))
            {
                if (this->mGoopLevel == 0xFF)
                {
                    this->mGoopLevel = 0xFF;
                }
                else
                {
                    this->mGoopLevel += 1;
                }
            }
        }
        else
        {
            this->setPoundingStatus(TDarkZhine::RISING);
            this->mPosition.y += 1;
            this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
        }
    }
    else if (this->poundingStatus() == TDarkZhine::RISING)
    {
        f32 averageFloorHeight = (this->mFloorBelow->mMaxHeight + this->mFloorBelow->mMinHeight) / 2;
        this->mSpeed.x = 0.0f;
        this->mSpeed.z = 0.0f;

        if (this->mPosition.y - averageFloorHeight > 500.0f &&
            this->mPosition.y > this->boundingPoint().y + 100.0f)
        {

            this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
            this->setPoundingStatus(TDarkZhine::INACTIVE);
        }
    }
    else
    {
        this->setPoundingStatus(TDarkZhine::INACTIVE);
    }
    this->setStatusTimer(this->statusTimer() - 1);
    return this->poundingStatus();
}

void TDarkZhine::advanceGoopDroplet()
{
    TBGPolDrop *pollutionDrop = this->mPollutionDrop;
    JGeometry::TVec3<f32> launchVelocity(this->mSpeed.x,
                                            this->mSpeed.y/2,
                                            this->mSpeed.z);

    if (pollutionDrop->mStatus == TBGPolDrop::DEAD)
    {
        launch__10TBGPolDropFRCQ29JGeometry8TVec3_f(this->mPollutionDrop,
                                                    this->mPosition,
                                                    launchVelocity);
    }
    else
    {
        move__10TBGPolDropFv(pollutionDrop);
    }
}

void TDarkZhine::perform(TMario *player)
{
    f32 targetAngle;
    f32 averageFloorHeight = (this->mFloorBelow->mMaxHeight + this->mFloorBelow->mMinHeight)/2;

    if (!this->isPounding())
    {
        if (this->isGooping())
            this->advanceGoopDroplet();

        if (this->mPosition.y - averageFloorHeight < 700.0f)
        {
            if (this->mPosition.y - this->mFloorBelow->mMaxHeight < 10.0f)
            {
                this->mPosition.y += 1.0f;
            }
            this->mSpeed.y = 1.0f * this->speedMultiplier();
        }
        else if (this->mPosition.y - averageFloorHeight > 700.0f)
        {
            if (this->mPosition.y < this->boundingPoint().y - this->boundingRadius())
            {
                this->mPosition.y = this->boundingPoint().y - this->boundingRadius();
                this->mSpeed.x = 0.0f;
            }
            else
            {
                this->mSpeed.y = -1.0f * this->speedMultiplier();
            }
        }
    }

    if (isTargetInRangeToHome(this->mPosition, this->shockRadius()) && !this->isPounding())
    {
        this->setIsFollowMario(true);
        this->setPoundingTimer(this->poundingTimer() - 1);
        this->setGoopingTimer(this->goopingTimer() - 1);
        this->mGravity = 0;
    }
    else
    {
        this->setIsFollowMario(false);
    }

    if (this->poundingStatus() == TDarkZhine::DROPPING || this->poundingStatus() == TDarkZhine::INACTIVE)
        moveObject__10TLiveActorFv(this);

    if (this->isFollowMario() == true)
    {
        this->setPoundingTimer(this->poundingTimer() - 1);
        this->setGoopingTimer(this->goopingTimer() - 1);

        if (this->poundingTimer() <= 0 && this->mPosition.y - averageFloorHeight < this->maxPoundingHeight())
        {
            this->setIsPounding(true);
            this->setPoundingStatus(TDarkZhine::DROPPING);
            this->setPoundingTimer(this->poundingTimerMax());
        }

        if (this->goopingTimer() <= 0)
        {
            this->setIsGooping(true);
            this->setGoopingTimer(this->goopingTimerMax());
        }

        this->mGroundSpeed += (this->accelerationRate() * this->mSpeedMultiplier);
        if (this->mGroundSpeed > this->mMaxSpeed)
            this->mGroundSpeed = this->mMaxSpeed;

        this->mSpeed.x = this->mGroundSpeed * sinf(AngleToRadians(this->mRotation.y));
        this->mSpeed.z = this->mGroundSpeed * cosf(AngleToRadians(this->mRotation.y));
    }
    else if (isTargetInRangeToHome(this->mPosition, this->shockRadius()) &&
                this->poundingStatus() != TDarkZhine::GROUNDROLL)
    {
        this->mGroundSpeed -= (this->accelerationRate() * this->mSpeedMultiplier);
        if (this->mGroundSpeed < 0.0f)
            this->mGroundSpeed = 0.0f;

        this->mSpeed.x = this->mGroundSpeed * sinf(AngleToRadians(this->mRotation.y));
        this->mSpeed.z = this->mGroundSpeed * cosf(AngleToRadians(this->mRotation.y));
    }

    if (isTargetInRangeToHome(this->mPosition, this->shockRadius()))
    {
        if (this->isPounding() && this->advanceDropAttack(gpPollution, player) == TDarkZhine::INACTIVE)
            this->setIsPounding(false);
        else
            this->mGravity = 0.0f;
    }
    else
    {
        this->setIsPounding(false);
        this->setPoundingStatus(TDarkZhine::INACTIVE);
        this->mPosition = this->boundingPoint();
    }
}

//0x80079B44
/*
void TDarkZhine::init()
{
    ZhineFile *gessoZhineFile = (ZhineFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc("scene"), "/bgeso/zhine.data");
    this->mGoopLevel = 0xE6;

    if (gessoZhineFile)
    {

        u32 *newVTable = (u32 *)Memory::malloc(0x120, 32);
        memcpy(newVTable, this->vTable, 0x114);
        newVTable[0xD0 / 4] = (u32)gessoZhineFile + (u32)gessoZhineFile->mCodeBlock[0]; //Replace move vtable entry
        *(u32 *)0x803B2A94 = (u32)gessoZhineFile + (u32)gessoZhineFile->mCodeBlock[1];  //Replace eye damage vtable entry

        this->setBoundingPoint(gessoZhineFile->ZhineBinData.mBoundingPoint);
        this->setBoundingRadius(gessoZhineFile->ZhineBinData.mBoundingAreaRadius);
        this->setShockRadius(gessoZhineFile->ZhineBinData.mShockRadius);
        this->setStampRadius(gessoZhineFile->ZhineBinData.mStampRadius);
        this->setMarioDistance(gessoZhineFile->ZhineBinData.mDistanceFromMario);
        this->setSpeedMultiplier(gessoZhineFile->ZhineBinData.mSpeedMultiplier);
        this->setAccelerationRate(gessoZhineFile->ZhineBinData.mAccelerationRate);
        this->setMaxSpeed(gessoZhineFile->ZhineBinData.mMaxSpeed);
        this->setFramesToCleanOnce(gessoZhineFile->ZhineBinData.mFramesToCleanOnce);
        this->setPoundingTimer(gessoZhineFile->ZhineBinData.mPoundingTimerMax);
        this->setShockingTimer(gessoZhineFile->ZhineBinData.mShockingTimerMax);
        this->setRollingTimer(gessoZhineFile->ZhineBinData.mRollingTimerMax);
        this->setGoopingTimer(gessoZhineFile->ZhineBinData.mGoopingTimerMax);
        this->setPoundingTimerMax(gessoZhineFile->ZhineBinData.mPoundingTimerMax);
        this->setShockingTimerMax(gessoZhineFile->ZhineBinData.mShockingTimerMax);
        this->setRollingTimerMax(gessoZhineFile->ZhineBinData.mRollingTimerMax);
        this->setGoopingTimerMax(gessoZhineFile->ZhineBinData.mGoopingTimerMax);
        this->setRisingRate(gessoZhineFile->ZhineBinData.mRisingRate);
        this->setMaxPoundingHeight(gessoZhineFile->ZhineBinData.mMaxPoundingHeight);

        this->vTable = newVTable;
        *(f32 *)0x8041014C = gessoZhineFile->ZhineBinData.mBoundingAreaRadius;
        *(f32 *)0x80410150 *= gessoZhineFile->ZhineBinData.mSpeedMultiplier;

        this->mGoopLevel = gessoZhineFile->ZhineBinData.mGoopLevel;
        this->setIsDarkZhine(true);
        OSReport("BossGesso set to Zhine mode");
    }
    else
    {
        this->setIsDarkZhine(false);
        OSReport("BossGesso set to Normal mode");
    }
    initAnmSound__10TLiveActorFv(this);
    return;
}

TDarkZhine *TDarkZhine::destroy()
{
    //use r6 and up
    *(u32 *)0x803B2A94 = 0x80074E54; //restore eye damage vtable entry
    *(f32 *)0x8041014C = 800.0f;
    *(f32 *)0x80410150 = 3000.0f;
    return this;
}
*/

//0x80005004
u32 TDarkZhine::cleanFromSpineBase(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase)
{
    TDarkZhine *gpDarkZhine = (TDarkZhine *)gpSpineBase->mTarget;
    if (gpDarkZhine->cleaningTimer() < gpDarkZhine->framesToCleanOnce())
    {
        gpDarkZhine->setCleaningTimer(gpDarkZhine->cleaningTimer() + 1);
        return 0;
    }
    else
    {
        gpDarkZhine->setCleaningTimer(0);
        return execute__17TNerveBGEyeDamageCFP24TSpineBase_1(gpNerveBGEye, gpSpineBase);
    }
}

//0x800764CC
/*
void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics)
{
    TDarkZhine *gesso;
    __asm { mr zhine, r31 };

    if (!gesso->isDarkZhine())
    {
        testPerform__Q26JDrama8TViewObjFUlPQ26JDrama9TGraphics(TBGTentacle, unk1, JDramaGraphics);
    }
}
kmCall(0x800764CC, &canUtilizeTentacles);
*/

//0x80005000
void control(TDarkZhine *thisZhine)
{
    thisZhine->perform(gpMarioAddress);
}

//0x800FFFAC = remove tentacles BLR, 0x800764CC perform tentacles?

//0x802A9D54 = TBossGesso Class Size
