#include "sms/JGeometry.hxx"
#include "sms/enemy/BossGesso.hxx"

#include "SME.hxx"

/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

class TDarkZhine
{

    struct ZhineFile
    {
        u32 *mCodeBlock[8]; //0x0000
        u32 mBlockSize[8];  //0x0020

        struct
        {
            JGeometry::TVec3<f32> mBoundingPoint; //0x0040
            f32 mBoundingAreaRadius;              //0x004C
            f32 mDistanceFromMario;               //0x0050
            f32 mSpeedMultiplier;                 //0x0054
            f32 mAccelerationRate;                //0x0058
            f32 mShockRadius;                     //0x005C
            f32 mMaxSpeed;                        //0x0060
            f32 mStampRadius;                     //0x0064
            f32 mRisingRate;                      //0x0068
            f32 mMaxPoundingHeight;               //0x006C
            s16 mPoundingTimerMax;                  //0x0070
            s16 mGoopingTimerMax;                   //0x0072
            s16 mShockingTimerMax;                  //0x0074
            s16 mRollingTimerMax;                   //0x0076
            s16 mFramesToCleanOnce;                 //0x0078
            u8 mGoopLevel;                          //0x007A
        } ZhineBinData;
    };

    enum PoundingState
    {
        INACTIVE,
        DROPPING,
        SHOCKING,
        GROUNDROLL,
        RISING
    };

    TBossGesso mBossInfo;
    JGeometry::TVec3<f32> mBoundingPoint; //0x01B0
    f32 mGroundSpeed;                     //0x01BC
    f32 mBoundingAreaRadius;              //0x01C0
    f32 mDistanceFromMario;
    f32 mSpeedMultiplier;
    f32 mAccelerationRate;
    f32 mShockRadius;
    f32 mMaxSpeed;
    f32 mStampRadius;
    bool mIsFollowMario;
    bool mIsPounding;
    bool mIsGooping;
    bool mIsShocking;
    s16 mPoundingTimer;
    s16 mGoopingTimer;
    s16 mShockingTimer;
    s16 mRollingTimer;
    s16 mStatusTimer;
    s16 mCleaningTimer;
    s16 mFramesToCleanOnce;
    s16 mPoundingTimerMax;
    s16 mGoopingTimerMax;
    s16 mShockingTimerMax;
    s16 mRollingTimerMax;
    PoundingState mPoundingStatus;
    bool mIsDarkZhine;
    f32 mRisingRate;
    f32 mMaxPoundingHeight;
    JDrama::TActor *mTarget;

public:
    void setBoundingPoint(JGeometry::TVec3<f32> *vec) { mBoundingPoint = *vec; }
    void setGroundSpeed(f32 vec) { mGroundSpeed = vec; }
    void setBoundingRadius(f32 r) { mBoundingAreaRadius = r; }
    void setMarioDistance(f32 d) { mDistanceFromMario = d; }
    void setSpeedMultiplier(f32 speedm) { mSpeedMultiplier = speedm; }
    void setAccelerationRate(f32 accel) { mAccelerationRate = accel; }
    void setShockRadius(f32 r) { mShockRadius = r; }
    void setMaxSpeed(f32 speed) { mMaxSpeed = speed; }
    void setStampRadius(f32 r) { mStampRadius = r; }
    void setFollowMario(bool state) { mIsFollowMario = state; }
    void setPounding(bool state) { mIsPounding = state; }
    void setGooping(bool state) { mIsGooping = state; }
    void setShocking(bool state) { mIsShocking = state; }
    void setPoundingTimer(u16 timer) { mPoundingTimer = timer; }
    void setGoopingTimer(u16 timer) { mGoopingTimer = timer; }
    void setShockingTimer(u16 timer) { mShockingTimer = timer; }
    void setRollingTimer(u16 timer) { mRollingTimer = timer; }
    void setStatusTimer(u16 timer) { mStatusTimer = timer; }
    void setCleaningTimer(u16 timer) { mCleaningTimer = timer; }
    void setFramesToCleanOnce(u16 timer) { mFramesToCleanOnce = timer; }
    void setPoundingTimerMax(u16 timer) { mPoundingTimerMax = timer; }
    void setGoopingTimerMax(u16 timer) { mGoopingTimerMax = timer; }
    void setShockingTimerMax(u16 timer) { mShockingTimerMax = timer; }
    void setRollingTimerMax(u16 timer) { mRollingTimerMax = timer; }
    void setPoundingStatus(PoundingState state) { mPoundingStatus = state; }
    void setDarkZhine(bool state) { mIsDarkZhine = state; }
    void setRisingRate(f32 rate) { mRisingRate = rate; }
    void setMaxPoundingHeight(f32 m) { mMaxPoundingHeight = m; }

    JGeometry::TVec3<f32> getBoundingPoint() { return mBoundingPoint; }
    f32 getGroundSpeed() { return mGroundSpeed; }
    f32 getBoundingRadius() { return mBoundingAreaRadius; }
    f32 getMarioDistance() { return mDistanceFromMario; }
    f32 getSpeedMultiplier() { return mSpeedMultiplier; }
    f32 getAccelerationRate() { return mAccelerationRate; }
    f32 getShockRadius() { return mShockRadius; }
    f32 getMaxSpeed() { return mMaxSpeed; }
    f32 getStampRadius() { return mStampRadius; }
    bool isFollowMario() { return mIsFollowMario; }
    bool isPounding() { return mIsPounding; }
    bool isGooping() { return mIsGooping; }
    bool isShocking() { return mIsShocking; }
    s16 getPoundingTimer() { return mPoundingTimer; }
    s16 getGoopingTimer() { return mGoopingTimer; }
    s16 getShockingTimer() { return mShockingTimer; }
    s16 getRollingTimer() { return mRollingTimer; }
    s16 getStatusTimer() { return mStatusTimer; }
    s16 getCleaningTimer() { return mCleaningTimer; }
    s16 getFramesToCleanOnce() { return mFramesToCleanOnce; }
    s16 getPoundingTimerMax() { return mPoundingTimerMax; }
    s16 getGoopingTimerMax() { return mGoopingTimerMax; }
    s16 getShockingTimerMax() { return mShockingTimerMax; }
    s16 getRollingTimerMax() { return mRollingTimerMax; }
    PoundingState getPoundingStatus() { return mPoundingStatus; }
    bool isDarkZhine() { return mIsDarkZhine; }
    f32 getRisingRate() { return mRisingRate; }
    f32 getMaxPoundingHeight() { return mMaxPoundingHeight; }

    f32 getAngleToTarget()
    {
        JGeometry::TVec3<f32> zhineCoordinates = mBossInfo.mPosition;

        return atan2f(this->mTarget->mPosition.x - zhineCoordinates.x,
                      this->mTarget->mPosition.z - zhineCoordinates.z);
    }

    bool isTargetInRangeToHome(JGeometry::TVec3<f32> home, f32 r)
    {
        return (fabsf(this->mTarget->mPosition.x - home.x) +
                fabsf(this->mTarget->mPosition.y - home.y) +
                fabsf(this->mTarget->mPosition.z - home.z) <
                r);
    }

    void advanceRollMovement(TPollutionManager *gpPollution)
    {

        if (getRollingTimer() < getRollingTimerMax()/2)
            this->mGroundSpeed = sigmoidCurve(this->getRollingTimer(),
                                              0.0f,
                                              this->getMaxSpeed() * this->getSpeedMultiplier(),
                                              300.0f,
                                              0.04f * this->getAccelerationRate());
        else
            this->mGroundSpeed = sigmoidCurve(this->getRollingTimer() - this->getRollingTimerMax()/2, 0, this->getMaxSpeed()*this->getSpeedMultiplier(), 300.0f, -0.04f*this->getAccelerationRate());

        if (this->mGroundSpeed > this->getMaxSpeed())
            this->mGroundSpeed = this->getMaxSpeed();
        else if (this->mGroundSpeed < 0.0f)
            this->mGroundSpeed = 0.0f;

        this->mBossInfo.mPosition.x += (this->mGroundSpeed * sinf(AngleToRadians(this->mBossInfo.mRotation.y)));
        this->mBossInfo.mPosition.z += (this->mGroundSpeed * cosf(AngleToRadians(this->mBossInfo.mRotation.y)));
        this->mBossInfo.mSpeed.x = (this->mGroundSpeed * sinf(AngleToRadians(this->mBossInfo.mRotation.y)));
        this->mBossInfo.mSpeed.z = (this->mGroundSpeed * cosf(AngleToRadians(this->mBossInfo.mRotation.y)));

        stamp__17TPollutionManagerFUsffff(gpPollution, 1,
                                          this->mBossInfo.mPosition.x,
                                          this->mBossInfo.mPosition.y,
                                          this->mBossInfo.mPosition.z,
                                          400.0f);
    }

    PoundingState advanceDropAttack(TPollutionManager *gpPollution, TMario *player)
    {

        if (getPoundingStatus() == TDarkZhine::DROPPING)
        {
            this->mBossInfo.mGravity = 1.0f;
            this->mBossInfo.mSpeed.x = 0.0f;
            this->mBossInfo.mSpeed.z = 0.0f;
            this->mBossInfo.mSpeed.y *= this->mSpeedMultiplier;

            if (this->mBossInfo.mPosition.y - this->mBossInfo.mFloorBelow->mMaxHeight < 10.0f &&
                this->mBossInfo.mPosition.y >= this->mBossInfo.mFloorBelow->mMinHeight)
            {

                startShake__12TCameraShakeF16EnumCamShakeModef(gpCameraShake, 0x27, 1.0f);
                startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(6158, this->mBossInfo.mPosition,
                                                                                       0, 0, 0, 4);
                start__9RumbleMgrFiP3Vec(gpPad1RumbleMgr, 8, this->mBossInfo.mPosition);
                stamp__17TPollutionManagerFUsffff(gpPollution, 1,
                                                  this->mBossInfo.mPosition.x,
                                                  this->mBossInfo.mFloorBelow->mMaxHeight,
                                                  this->mBossInfo.mPosition.z,
                                                  mStampRadius);

                if (this->mBossInfo.mGoopLevel > (0xFF - 0x30))
                {
                    this->mBossInfo.mGoopLevel = 0xFF;
                }
                else
                {
                    this->mBossInfo.mGoopLevel += 0x20;
                }

                this->setPoundingStatus(TDarkZhine::SHOCKING);
                this->setStatusTimer(getShockingTimerMax());
            }
        }
        else if (this->getPoundingStatus() == TDarkZhine::SHOCKING)
        {
            if (!(player->mState & TMario::SA_AIRBORN) &&
                this->isTargetInRangeToHome(this->mBossInfo.mPosition, this->getShockRadius()) &&
                (player->mState != TMario::SA_KNCK_LND &&
                 player->mState != 0x4045C))
            {
                decHP__6TMarioFi(player, 1);
                changePlayerStatus__6TMarioFUlUlb(player, TMario::SA_KNCK_LND, 0, 0);
            }

            if (this->getStatusTimer() <= 0)
            {
                this->setPoundingStatus(TDarkZhine::GROUNDROLL);
                this->setStatusTimer(this->getRollingTimerMax());
            }
        }
        else if (this->getPoundingStatus() == TDarkZhine::GROUNDROLL)
        {
            if (this->mBossInfo.mPosition.y - this->mBossInfo.mFloorBelow->mMaxHeight > 100)
            {
                this->setPoundingStatus(TDarkZhine::RISING);
                this->mBossInfo.mGravity *= (-1 * this->getRisingRate() * this->getSpeedMultiplier());
            }
            else if (getRollingTimer() > 0)
            {
                this->advanceRollMovement(gpPollution);
                if (!(this->getStatusTimer() % 15))
                {
                    if (this->mBossInfo.mGoopLevel == 0xFF)
                    {
                        this->mBossInfo.mGoopLevel = 0xFF;
                    }
                    else
                    {
                        this->mBossInfo.mGoopLevel += 1;
                    }
                }
            }
            else
            {
                this->setPoundingStatus(TDarkZhine::RISING);
                this->mBossInfo.mPosition.y += 1;
                this->mBossInfo.mGravity *= (-1 * this->getRisingRate() * this->getSpeedMultiplier());
            }
        }
        else if (this->getPoundingStatus() == TDarkZhine::RISING)
        {
            f32 averageFloorHeight = (this->mBossInfo.mFloorBelow->mMaxHeight + this->mBossInfo.mFloorBelow->mMinHeight) / 2;
            this->mBossInfo.mSpeed.x = 0.0f;
            this->mBossInfo.mSpeed.z = 0.0f;

            if (this->mBossInfo.mPosition.y - averageFloorHeight > 500.0f &&
                this->mBossInfo.mPosition.y > this->getBoundingPoint().y + 100.0f)
            {

                this->mBossInfo.mGravity *= (-1 * this->getRisingRate() * this->getSpeedMultiplier());
                this->setPoundingStatus(TDarkZhine::INACTIVE);
            }
        }
        else
        {
            this->setPoundingStatus(TDarkZhine::INACTIVE);
        }
        this->setStatusTimer(getStatusTimer() - 1);
        return this->getPoundingStatus();
    }

    void advanceGoopDroplet()
    {
        TBGPolDrop *pollutionDrop = this->mBossInfo.mPollutionDrop;
        JGeometry::TVec3<f32> launchVelocity(this->mBossInfo.mSpeed.x,
                                               this->mBossInfo.mSpeed.y/2,
                                               this->mBossInfo.mSpeed.z);

        if (pollutionDrop->mStatus == TBGPolDrop::DEAD)
        {
            launch__10TBGPolDropFRCQ29JGeometry8TVec3_f(this->mBossInfo.mPollutionDrop,
                                                        this->mBossInfo.mPosition,
                                                        launchVelocity);
        }
        else
        {
            move__10TBGPolDropFv(pollutionDrop);
        }
    }

    void update(TMario *player)
    {
        f32 targetAngle;
        f32 averageFloorHeight = (this->mBossInfo.mFloorBelow->mMaxHeight + this->mBossInfo.mFloorBelow->mMinHeight)/2;

        if (!this->isPounding())
        {
            if (this->isGooping())
                this->advanceGoopDroplet();

            if (this->mBossInfo.mPosition.y - averageFloorHeight < 700.0f)
            {
                if (this->mBossInfo.mPosition.y - this->mBossInfo.mFloorBelow->mMaxHeight < 10.0f)
                {
                    this->mBossInfo.mPosition.y += 1.0f;
                }
                this->mBossInfo.mSpeed.y = 1.0f * this->getSpeedMultiplier();
            }
            else if (this->mBossInfo.mPosition.y - averageFloorHeight > 700.0f)
            {
                if (this->mBossInfo.mPosition.y < this->getBoundingPoint().y - this->getBoundingRadius())
                {
                    this->mBossInfo.mPosition.y = this->getBoundingPoint().y - this->getBoundingRadius();
                    this->mBossInfo.mSpeed.x = 0.0f;
                }
                else
                {
                    this->mBossInfo.mSpeed.y = -1.0f * this->getSpeedMultiplier();
                }
            }
        }

        if (isTargetInRangeToHome(this->mBossInfo.mPosition, this->getShockRadius()) && !this->isPounding())
        {
            this->setFollowMario(true);
            this->setPoundingTimer(this->getPoundingTimer() - 1);
            this->setGoopingTimer(this->getGoopingTimer() - 1);
            this->mBossInfo.mGravity = 0;
        }
        else
        {
            this->setFollowMario(false);
        }

        if (this->getPoundingStatus() == TDarkZhine::DROPPING || this->getPoundingStatus() == TDarkZhine::INACTIVE)
            moveObject__10TLiveActorFv(this);

        if (this->isFollowMario() == true)
        {
            this->setPoundingTimer(getPoundingTimer() - 1);
            this->setGoopingTimer(getGoopingTimer() - 1);

            if (this->getPoundingTimer() <= 0 && this->mBossInfo.mPosition.y - averageFloorHeight < this->getMaxPoundingHeight())
            {
                this->setPounding(true);
                this->setPoundingStatus(TDarkZhine::DROPPING);
                this->setPoundingTimer(getPoundingTimerMax());
            }

            if (getGoopingTimer() <= 0)
            {
                this->setGooping(true);
                this->setGoopingTimer(this->getGoopingTimerMax());
            }

            this->mGroundSpeed += (this->getAccelerationRate() * this->mSpeedMultiplier);
            if (this->mGroundSpeed > this->mMaxSpeed)
                this->mGroundSpeed = this->mMaxSpeed;

            this->mBossInfo.mSpeed.x = this->mGroundSpeed * sinf(AngleToRadians(this->mBossInfo.mRotation.y));
            this->mBossInfo.mSpeed.z = this->mGroundSpeed * cosf(AngleToRadians(this->mBossInfo.mRotation.y));
        }
        else if (isTargetInRangeToHome(this->mBossInfo.mPosition, getShockRadius()) &&
                 getPoundingStatus() != TDarkZhine::GROUNDROLL)
        {
            this->mGroundSpeed -= (getAccelerationRate() * this->mSpeedMultiplier);
            if (this->mGroundSpeed < 0.0f)
                this->mGroundSpeed = 0.0f;

            this->mBossInfo.mSpeed.x = this->mGroundSpeed * sinf(AngleToRadians(this->mBossInfo.mRotation.y));
            this->mBossInfo.mSpeed.z = this->mGroundSpeed * cosf(AngleToRadians(this->mBossInfo.mRotation.y));
        }

        if (isTargetInRangeToHome(this->mBossInfo.mPosition, getShockRadius()))
        {
            if (this->isPounding() && this->advanceDropAttack(gpPollution, player) == TDarkZhine::INACTIVE)
                this->setPounding(false);
            else
                this->mBossInfo.mGravity = 0.0f;
        }
        else
        {
            this->setPounding(false);
            this->setPoundingStatus(TDarkZhine::INACTIVE);
            this->mBossInfo.mPosition = this->getBoundingPoint();
        }
    }

    //0x80079B44
    void init()
    {
        ZhineFile *gessoZhineFile = (ZhineFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc("scene"), "/bgeso/zhine.data");
        this->mBossInfo.mGoopLevel = 0xE6;

        if (gessoZhineFile)
        {

            u32 *newVTable = (u32 *)Memory::malloc(0x120, 32);
            memcpy(newVTable, this->mBossInfo.vTable, 0x114);
            newVTable[0xD0 / 4] = (u32)gessoZhineFile + (u32)gessoZhineFile->mCodeBlock[0]; //Replace move vtable entry
            *(u32 *)0x803B2A94 = (u32)gessoZhineFile + (u32)gessoZhineFile->mCodeBlock[1];  //Replace eye damage vtable entry

            this->setBoundingPoint(&gessoZhineFile->ZhineBinData.mBoundingPoint);
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

            this->mBossInfo.vTable = newVTable;
            *(f32 *)0x8041014C = gessoZhineFile->ZhineBinData.mBoundingAreaRadius;
            *(f32 *)0x80410150 *= gessoZhineFile->ZhineBinData.mSpeedMultiplier;

            this->mBossInfo.mGoopLevel = gessoZhineFile->ZhineBinData.mGoopLevel;
            this->setDarkZhine(true);
            OSReport("BossGesso set to Zhine mode");
        }
        else
        {
            this->setDarkZhine(false);
            OSReport("BossGesso set to Normal mode");
        }
        initAnmSound__10TLiveActorFv(this);
        return;
    }

    TDarkZhine *destroy()
    {
        //use r6 and up
        *(u32 *)0x803B2A94 = 0x80074E54; //restore eye damage vtable entry
        *(f32 *)0x8041014C = 800.0f;
        *(f32 *)0x80410150 = 3000.0f;
        return this;
    }
};

//0x80005004
u32 cleanZhine(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase)
{
    TDarkZhine *gpDarkZhine = (TDarkZhine *)gpSpineBase->mTarget;
    if (gpDarkZhine->getCleaningTimer() < gpDarkZhine->getFramesToCleanOnce())
    {
        gpDarkZhine->setCleaningTimer(gpDarkZhine->getCleaningTimer() + 1);
        return 0;
    }
    else
    {
        gpDarkZhine->setCleaningTimer(0);
        return execute__17TNerveBGEyeDamageCFP24TSpineBase_1(gpNerveBGEye, gpSpineBase);
    }
}

//0x800764CC
void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics, TDarkZhine *thisZhine)
{
    //mr r6, r31
    if (!thisZhine->isDarkZhine())
    {
        testPerform__Q26JDrama8TViewObjFUlPQ26JDrama9TGraphics(TBGTentacle, unk1, JDramaGraphics);
    }
}

//0x80005000
void control(TDarkZhine *thisZhine)
{
    thisZhine->update(gpMarioAddress);
}

//0x800FFFAC = remove tentacles BLR, 0x800764CC perform tentacles?

//0x802A9D54 = TBossGesso Class Size
