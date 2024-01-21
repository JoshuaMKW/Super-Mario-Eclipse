#pragma once

#include <Dolphin/types.h>

#include <JSystem/JGeometry/JGMVec.hxx>
#include <SMS/Enemy/BossGesso.hxx>

#include <BetterSMS/module.hxx>
#include <Manager/PollutionManager.hxx>

#define ZHINE_POL_DROP_COUNT 4

class TDarkZhine;

// struct TZhineParams : public TParams {
// #def ine CONSTRUCT_PARAM(name, val) \
//    name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)), SME_STRINGIZE(name))
//
//     TZhineParams(const char *prm)
//         : TParams(), CONSTRUCT_PARAM(mBoundingAreaRadius, 1000.0f),
//           CONSTRUCT_PARAM(mDistanceFromMario, 2000.0f), CONSTRUCT_PARAM(mSpeedMultiplier, 1.0f),
//           CONSTRUCT_PARAM(mAccelerationRate, 0.98f), CONSTRUCT_PARAM(mShockRadius, 2000.0f),
//           CONSTRUCT_PARAM(mMaxSpeed, 60.0f), CONSTRUCT_PARAM(mStampRadius, 1000.0f),
//           CONSTRUCT_PARAM(mFramesToCleanOnce, 3), CONSTRUCT_PARAM(mPoundingTimerMax, 300),
//           CONSTRUCT_PARAM(mGoopingTimerMax, 0), CONSTRUCT_PARAM(mShockingTimerMax, 120),
//           CONSTRUCT_PARAM(mRollingTimerMax, 300), CONSTRUCT_PARAM(mHardMode, false),
//           CONSTRUCT_PARAM(mRisingRate, 1.4f), CONSTRUCT_PARAM(mMaxPoundingHeight, 800.0f) {
//         load(prm);
//     }
//
// #undef CONSTRUCT_PARAM
//
//     TParamRT<f32> mBoundingAreaRadius;
//     TParamRT<f32> mDistanceFromMario;
//     TParamRT<f32> mSpeedMultiplier;
//     TParamRT<f32> mAccelerationRate;
//     TParamRT<f32> mShockRadius;
//     TParamRT<f32> mMaxSpeed;
//     TParamRT<f32> mStampRadius;
//     TParamRT<s16> mFramesToCleanOnce;
//     TParamRT<s16> mPoundingTimerMax;
//     TParamRT<s16> mGoopingTimerMax;
//     TParamRT<s16> mShockingTimerMax;
//     TParamRT<s16> mRollingTimerMax;
//     TParamRT<bool> mHardMode;
//     TParamRT<f32> mRisingRate;
//     TParamRT<f32> mMaxPoundingHeight;
// };

class TNerveZhineSleep : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineSleep(){};
    virtual ~TNerveZhineSleep() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineWake : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineWake(){};
    virtual ~TNerveZhineWake() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineFly : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineFly(){};
    virtual ~TNerveZhineFly() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhinePound : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhinePound(){};
    virtual ~TNerveZhinePound() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineRoll : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineRoll(){};
    virtual ~TNerveZhineRoll() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineRise : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineRise(){};
    virtual ~TNerveZhineRise() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TDarkZhine : public TSpineEnemy {
    enum PoundingState { INACTIVE, DROPPING, SHOCKING, GROUNDROLL, RISING };

public:
    TDarkZhine(const char *);
    virtual ~TDarkZhine();

    void load(JSUMemoryInputStream &) override;
    void perform(u32, JDrama::TGraphics *) override;
    bool receiveMessage(THitActor *, u32) override;
    void init(TLiveManager *) override;
    void control() override;
    void bind() override;
    void moveObject() override;
    const char **getBasNameTable() const;

    f32 getAngleToTarget() const;

    bool isFollowMario() const { return mIsFollowMario; }
    bool isPounding() const { return mIsPounding; }
    bool isGooping() const { return mIsGooping; }
    bool isShocking() const { return mIsShocking; }
    bool isTargetInRangeToHome(f32 range) const;

    void setBckAnm(int index) {
        mCurAnmIndex = index;
        mActorData->setBckFromIndex(index);
        const char **bas_table = getBasNameTable();
        if (bas_table) {
            setAnmSound(bas_table[index]);
        }
    }

    bool doGroundPound();
    void launchPolDrop();

    PoundingState advanceDropAttack(TPollutionManager *gpPollution, TMario *player);
    void advanceGoopDroplet();
    void advanceRollMovement(TPollutionManager *gpPollution);
    static u32 cleanFromSpineBase(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase);

private:
    TBGPolDrop mPolDrops[ZHINE_POL_DROP_COUNT];
    u8 mPolDropCurrent;

    int mCurAnmIndex;
    s32 mGroundPoundTimer;

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
};