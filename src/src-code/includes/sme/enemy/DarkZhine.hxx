#pragma once

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

class TDarkZhine : public TBossGesso
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
            s16 mPoundingTimerMax;                //0x0070
            s16 mGoopingTimerMax;                 //0x0072
            s16 mShockingTimerMax;                //0x0074
            s16 mRollingTimerMax;                 //0x0076
            s16 mFramesToCleanOnce;               //0x0078
            u8 mGoopLevel;                        //0x007A
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

    JGeometry::TVec3<f32> mBoundingPoint;
    f32 mGroundSpeed;
    f32 mBoundingAreaRadius;
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
    bool mHardMode;
    f32 mRisingRate;
    f32 mMaxPoundingHeight;
    JDrama::TActor *mTarget;

public:
    TDarkZhine(const char *);
    TDarkZhine(const char *, bool hardMode);
    ~TDarkZhine();

    void setAccelerationRate(f32 accel) { mAccelerationRate = accel; }
    void setBoundingPoint(JGeometry::TVec3<f32> &vec) { mBoundingPoint = vec; }
    void setBoundingPoint(f32 x, f32 y, f32 z) { mBoundingPoint.x = x; mBoundingPoint.y = y; mBoundingPoint.z = z; }
    void setBoundingRadius(f32 r) { mBoundingAreaRadius = r; }
    void setCleaningTimer(u16 timer) { mCleaningTimer = timer; }
    void setIsFollowMario(bool state) { mIsFollowMario = state; }
    void setFramesToCleanOnce(u16 timer) { mFramesToCleanOnce = timer; }
    void setIsGooping(bool state) { mIsGooping = state; }
    void setGoopingTimer(u16 timer) { mGoopingTimer = timer; }
    void setGoopingTimerMax(u16 timer) { mGoopingTimerMax = timer; }
    void setGroundSpeed(f32 vec) { mGroundSpeed = vec; }
    void setMarioDistance(f32 d) { mDistanceFromMario = d; }
    void setMaxPoundingHeight(f32 m) { mMaxPoundingHeight = m; }
    void setMaxSpeed(f32 speed) { mMaxSpeed = speed; }
    void setIsPounding(bool state) { mIsPounding = state; }
    void setPoundingStatus(PoundingState state) { mPoundingStatus = state; }
    void setPoundingTimer(u16 timer) { mPoundingTimer = timer; }
    void setPoundingTimerMax(u16 timer) { mPoundingTimerMax = timer; }
    void setRollingTimer(u16 timer) { mRollingTimer = timer; }
    void setRollingTimerMax(u16 timer) { mRollingTimerMax = timer; }
    void setIsShocking(bool state) { mIsShocking = state; }
    void setShockingTimer(u16 timer) { mShockingTimer = timer; }
    void setShockingTimerMax(u16 timer) { mShockingTimerMax = timer; }
    void setShockRadius(f32 r) { mShockRadius = r; }
    void setSpeedMultiplier(f32 speedm) { mSpeedMultiplier = speedm; }
    void setStampRadius(f32 r) { mStampRadius = r; }
    void setStatusTimer(u16 timer) { mStatusTimer = timer; }
    void setRisingRate(f32 rate) { mRisingRate = rate; }

    f32 accelerationRate() const { return this->mAccelerationRate; }
    JGeometry::TVec3<f32> boundingPoint() const { return this->mBoundingPoint; }
    f32 boundingRadius() const { return this->mBoundingAreaRadius; }
    s16 cleaningTimer() const { return this->mCleaningTimer; }
    s16 framesToCleanOnce() const { return this->mFramesToCleanOnce; }
    s16 goopingTimer() const { return this->mGoopingTimer; }
    f32 groundSpeed() const { return this->mGroundSpeed; }
    s16 goopingTimerMax() const { return this->mGoopingTimerMax; }
    f32 marioDistance() const { return this->mDistanceFromMario; }
    f32 maxPoundingHeight() const { return this->mMaxPoundingHeight; }
    f32 maxSpeed() const { return this->mMaxSpeed; }
    PoundingState poundingStatus() const { return this->mPoundingStatus; }
    s16 poundingTimer() const { return this->mPoundingTimer; }
    s16 poundingTimerMax() const { return this->mPoundingTimerMax; }
    s16 rollingTimer() const { return this->mRollingTimer; }
    s16 rollingTimerMax() const { return this->mRollingTimerMax; }
    s16 shockingTimer() const { return this->mShockingTimer; }
    f32 shockRadius() const { return this->mShockRadius; }
    s16 shockingTimerMax() const { return this->mShockingTimerMax; }
    f32 speedMultiplier() const { return this->mSpeedMultiplier; }
    f32 stampRadius() const { return this->mStampRadius; }
    s16 statusTimer() const { return this->mStatusTimer; }
    f32 risingRate() const { return this->mRisingRate; }

    bool isFollowMario() const { return this->mIsFollowMario; }
    bool isPounding() const { return this->mIsPounding; }
    bool isGooping() const { return this->mIsGooping; }
    bool isShocking() const { return this->mIsShocking; }

    PoundingState advanceDropAttack(TPollutionManager *gpPollution, TMario *player);
    void advanceGoopDroplet();
    void advanceRollMovement(TPollutionManager *gpPollution);
    void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics, TDarkZhine *thisZhine);
    static u32 cleanFromSpineBase(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase);
    void control(TDarkZhine *thisZhine);
    f32 getAngleToTarget();
    bool isTargetInRangeToHome(JGeometry::TVec3<f32> home, f32 r);
    void perform(TMario *player);
};