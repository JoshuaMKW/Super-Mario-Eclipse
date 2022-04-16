#pragma once

#include "JGeometry.hxx"
#include "sms/enemy/BossGesso.hxx"
#include "sms/manager/EnemyManager.hxx"

#include "SME.hxx"
#include "types.h"

#if SME_ZHINE_BOSS

struct TBossDarkZhineParams : public TParams {
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  TBossDarkZhineParams(const char *prm)
      : TParams(), CONSTRUCT_PARAM(mDistanceFromMario, 2000.0f),
        CONSTRUCT_PARAM(mSpeedMultiplier, 1.0f),
        CONSTRUCT_PARAM(mAccelerationRate, 0.98f),
        CONSTRUCT_PARAM(mShockRadius, 2000.0f),
        CONSTRUCT_PARAM(mMaxSpeed, 60.0f), CONSTRUCT_PARAM(mTurnRate, 5.0f),
        CONSTRUCT_PARAM(mStampRadius, 1000.0f),
        CONSTRUCT_PARAM(mFramesToCleanOnce, 3),
        CONSTRUCT_PARAM(mPoundingTimerMax, 300),
        CONSTRUCT_PARAM(mGoopingTimerMax, 0),
        CONSTRUCT_PARAM(mShockingTimerMax, 120),
        CONSTRUCT_PARAM(mRollingTimerMax, 300),
        CONSTRUCT_PARAM(mHardMode, false), CONSTRUCT_PARAM(mRisingRate, 1.4f),
        CONSTRUCT_PARAM(mMaxPoundingHeight, 800.0f) {
    load(prm);
  }

#undef CONSTRUCT_PARAM
  TParamRT<f32> mDistanceFromMario;
  TParamRT<f32> mSpeedMultiplier;
  TParamRT<f32> mAccelerationRate;
  TParamRT<f32> mShockRadius;
  TParamRT<f32> mMaxSpeed;
  TParamRT<f32> mTurnRate;
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

class TBossDarkZhineManager : public TEnemyManager {
public:
  TBossDarkZhineManager(const char *);
  virtual ~TBossDarkZhineManager();
  
  virtual void load(JSUMemoryInputStream &) override;
  virtual void createModelData() override;

  void initJParticle();
};

class TBossDarkZhine : public TSpineEnemy {
  enum ActionState {
    IDLE,
    FLYING,
    TELEPORTING,
    DROPPING,
    SHOCKING,
    GROUNDROLL,
    RISING
  };

public:
  TBossDarkZhine(const char *);
  virtual ~TBossDarkZhine();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void init(TLiveManager *) override;
  virtual void reset() override;
  virtual void control() override;
  virtual void moveObject() override;
  virtual const char **getBasNameTable() const override;

  f32 getAngleToTarget() const;

  bool isFollowMario() const { return mIsFollowMario; }
  bool isPounding() const { return mIsPounding; }
  bool isGooping() const { return mIsGooping; }
  bool isShocking() const { return mIsShocking; }
  bool isInRange(const TVec3f &pos, f32 range) const;

private:
  bool advanceDropAttack();
  bool advanceGoopDroplet();
  bool advanceRollAttack();
  bool advanceFlying();
  bool advanceRising();
  void sleep();
  bool warpToPoint(const TVec3f &point);

  TVec3f mBoundingPoint;
  f32 mBoundingAreaRadius;
  f32 mForwardSpeed;
  ActionState mActionState;
  TMario *mTarget;
  TRingBuffer<TBGPolDrop> mPolDrops;
  s16 mStatusTimer;
  s16 mWarpingTimer;
  s16 mCleaningTimer;
  f32 mGoopCoverage;
  bool mIsFollowMario;
  bool mIsPounding;
  bool mIsGooping;
  bool mIsShocking;
};

#endif