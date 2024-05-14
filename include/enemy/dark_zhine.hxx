#pragma once

#include "Manager/EnemyManager.hxx"
#include "Manager/PollutionManager.hxx"

#include <Dolphin/types.h>

#include <JSystem/JGeometry/JGMVec.hxx>
#include <SMS/Enemy/BossGesso.hxx>

#include <BetterSMS/module.hxx>


/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

struct TZhineParams : public TParams {
    TZhineParams(const char *prm)
        : TParams(), SMS_TPARAM_INIT(mBoundingAreaRadius, 1000.0f),
          SMS_TPARAM_INIT(mDistanceFromMario, 2000.0f), SMS_TPARAM_INIT(mSpeedMultiplier, 1.0f),
          SMS_TPARAM_INIT(mAccelerationRate, 0.98f), SMS_TPARAM_INIT(mShockRadius, 2000.0f),
          SMS_TPARAM_INIT(mMaxSpeed, 60.0f), SMS_TPARAM_INIT(mStampRadius, 1000.0f),
          SMS_TPARAM_INIT(mFramesToCleanOnce, 3), SMS_TPARAM_INIT(mPoundingTimerMax, 300),
          SMS_TPARAM_INIT(mGoopingTimerMax, 0), SMS_TPARAM_INIT(mShockingTimerMax, 120),
          SMS_TPARAM_INIT(mRollingTimerMax, 300), SMS_TPARAM_INIT(mHardMode, false),
          SMS_TPARAM_INIT(mRisingRate, 1.4f), SMS_TPARAM_INIT(mMaxPoundingHeight, 800.0f) {
        load(prm);
    }

#undef CONSTRUCT_PARAM

    TParamRT<f32> mBoundingAreaRadius;
    TParamRT<f32> mDistanceFromMario;
    TParamRT<f32> mSpeedMultiplier;
    TParamRT<f32> mAccelerationRate;
    TParamRT<f32> mShockRadius;
    TParamRT<f32> mMaxSpeed;
    TParamRT<f32> mStampRadius;
    TParamRT<s16> mFramesToCleanOnce;
    TParamRT<s16> mPoundingTimerMax;
    TParamRT<s16> mGoopingTimerMax;
    TParamRT<s16> mShockingTimerMax;
    TParamRT<s16> mRollingTimerMax;
    TParamRT<bool> mHardMode;
    TParamRT<f32> mRisingRate;
    TParamRT<f32> mMaxPoundingHeight;
};

class TDarkZhine : public TSpineEnemy {
    enum PoundingState { INACTIVE, DROPPING, SHOCKING, GROUNDROLL, RISING };

    TVec3f mBoundingPoint;
    PoundingState mPoundingStatus;
    JDrama::TActor *mTarget;
    s16 mPoundingTimer;
    s16 mGoopingTimer;
    s16 mShockingTimer;
    s16 mRollingTimer;
    s16 mStatusTimer;
    s16 mCleaningTimer;
    bool mIsFollowMario;
    bool mIsPounding;
    bool mIsGooping;
    bool mIsShocking;
    TZhineParams mParams;
    TBGPolDrop *mPollutionDrop;

public:

    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TDarkZhine("DarkZhine");
    }

    TDarkZhine(const char *);
    virtual ~TDarkZhine();

    // void load(JSUMemoryInputStream &) override;
    void perform(u32, JDrama::TGraphics *) override;
    // bool receiveMessage(THitActor *, u32) override;
    void init(TLiveManager *) override;

    f32 getAngleToTarget() const;

    bool isFollowMario() const { return mIsFollowMario; }
    bool isPounding() const { return mIsPounding; }
    bool isGooping() const { return mIsGooping; }
    bool isShocking() const { return mIsShocking; }
    bool isTargetInRangeToHome(f32 range) const;

    PoundingState advanceDropAttack(TPollutionManager *gpPollution, TMario *player);
    void advanceGoopDroplet();
    void advanceRollMovement(TPollutionManager *gpPollution);
    void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics,
                             TDarkZhine *thisZhine);
    // static u32 cleanFromSpineBase(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase);
};

class TDarkZhineManager : public TEnemyManager {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TDarkZhineManager("DarkZhineManager");
    }

    TDarkZhineManager(char const *);
    virtual ~TDarkZhineManager(){};

    virtual void load(JSUMemoryInputStream &);
    virtual void createModelData();
    virtual void createAnmData();
};