#pragma once

#include "JGeometry.hxx"
#include "sms/actor/SpineEnemy.hxx"
#include "sms/enemy/BossGesso.hxx"
#include "sms/manager/EnemyManager.hxx"


#include "SME.hxx"
#include "types.h"

#if SME_ZHINE_BOSS

struct TBossDarkZhineParams : public TSpineEnemyParams {
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  TBossDarkZhineParams(const char *prm)
      : TSpineEnemyParams(prm), CONSTRUCT_PARAM(mSLDistanceFromMario, 2000.0f),
        CONSTRUCT_PARAM(mSLSpeedMultiplier, 1.0f),
        CONSTRUCT_PARAM(mSLAccelerationRate, 0.98f),
        CONSTRUCT_PARAM(mSLShockRadius, 2000.0f),
        CONSTRUCT_PARAM(mSLMaxSpeed, 60.0f), CONSTRUCT_PARAM(mSLTurnRate, 5.0f),
        CONSTRUCT_PARAM(mSLStampRadius, 1000.0f),
        CONSTRUCT_PARAM(mSLFramesToCleanOnce, 3),
        CONSTRUCT_PARAM(mSLPoundingTimerMax, 300),
        CONSTRUCT_PARAM(mSLGoopingTimerMax, 0),
        CONSTRUCT_PARAM(mSLShockingTimerMax, 120),
        CONSTRUCT_PARAM(mSLRollingTimerMax, 300),
        CONSTRUCT_PARAM(mSLHardMode, false),
        CONSTRUCT_PARAM(mSLRisingRate, 1.4f),
        CONSTRUCT_PARAM(mSLMaxPoundingHeight, 800.0f) {
    load(prm);
  }

#undef CONSTRUCT_PARAM
  TParamRT<f32> mSLDistanceFromMario;
  TParamRT<f32> mSLSpeedMultiplier;
  TParamRT<f32> mSLAccelerationRate;
  TParamRT<f32> mSLShockRadius;
  TParamRT<f32> mSLMaxSpeed;
  TParamRT<f32> mSLTurnRate;
  TParamRT<f32> mSLStampRadius;
  TParamRT<s16> mSLFramesToCleanOnce;
  TParamRT<s16> mSLPoundingTimerMax;
  TParamRT<s16> mSLGoopingTimerMax;
  TParamRT<s16> mSLShockingTimerMax;
  TParamRT<s16> mSLRollingTimerMax;
  TParamRT<bool> mSLHardMode;
  TParamRT<f32> mSLRisingRate;
  TParamRT<f32> mSLMaxPoundingHeight;
};

class TNerveBZWait : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZWait();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
};

class TNerveBZAppear : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZAppear();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
};

class TNerveBZSleep : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZSleep();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
};

class TNerveBZFly : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZFly();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
};

class TNerveBZPound : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZPound();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
};

class TNerveBZRoll : public TNerveBase<TLiveActor> {
public:
  virtual ~TNerveBZRoll();
  virtual bool execute(TSpineBase<TLiveActor> *spine) const override;
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
  friend TNerveBZWait;
  friend TNerveBZAppear;
  friend TNerveBZSleep;
  friend TNerveBZFly;
  friend TNerveBZPound;
  friend TNerveBZRoll;

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
  virtual void loadAfter() override;
  virtual void reset() override;
  virtual void control() override;
  virtual void moveObject() override;
  virtual const char **getBasNameTable() const override;

  bool isFollowMario() const { return mIsFollowMario; }
  bool isPounding() const { return mIsPounding; }
  bool isGooping() const { return mIsGooping; }
  bool isShocking() const { return mIsShocking; }
  bool isInRange(const TVec3f &pos, f32 range) const;
  // bool isIntroReady() const { return !mHasIntroHappened; }
  // bool setIntroReady(bool ready) { mHasIntroHappened = ready; }

  const TVec3f &getBoundingPoint() const { return mBoundingPoint; }
  f32 getBoundingRadius() const { return mBoundingAreaRadius; }
  TMario *getTargetPlayer() const { return mTarget; };
  TMario *getClosestPlayer() const;

protected:
  bool advanceDropAttack(s32 frame);
  bool advanceGoopDroplet(s32 frame);
  bool advanceRollAttack(s32 frame);
  bool advanceFlying(s32 frame);
  bool advanceRising(s32 frame);

  void sleep();
  bool warpToPoint(const TVec3f &point);
  void addGoopCoverage(f32 amount);
  void updateGoopKColor();

  TVec3f mBoundingPoint;
  f32 mBoundingAreaRadius;
  f32 mForwardSpeed;
  ActionState mActionState;
  TMario *mTarget;
  TRingBuffer<TBGPolDrop> mPolDrops;
  JPABaseEmitter *mPromiEffect; // Light Flare
  JPABaseEmitter *mSenkoEffect; // Light rays
  JPABaseEmitter *mKiraEffect;  // Sparkles
  JPABaseEmitter *mBowEffect;   // Light bow
  s16 mStatusTimer;
  s16 mWarpingTimer;
  s16 mCleaningTimer;
  f32 mGoopCoverage;
  bool mIsFollowMario;
  bool mIsPounding;
  bool mIsGooping;
  bool mIsShocking;
  bool mHasIntroHappened;
  s16 mIntroTimer;
};

#endif