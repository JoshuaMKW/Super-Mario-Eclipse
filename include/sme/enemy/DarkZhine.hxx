#pragma once

#include "JGeometry.hxx"
#include "sms/enemy/BossGesso.hxx"

#include "SME.hxx"
#include "types.h"


#if SME_ZHINE_BOSS

/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

struct TZhineParams : public TParams {
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  TZhineParams(const char *prm)
      : TParams(), CONSTRUCT_PARAM(mBoundingAreaRadius, 1000.0f),
        CONSTRUCT_PARAM(mDistanceFromMario, 2000.0f),
        CONSTRUCT_PARAM(mSpeedMultiplier, 1.0f),
        CONSTRUCT_PARAM(mAccelerationRate, 0.98f),
        CONSTRUCT_PARAM(mShockRadius, 2000.0f),
        CONSTRUCT_PARAM(mMaxSpeed, 60.0f),
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

public:
  TDarkZhine(const char *);
  virtual ~TDarkZhine();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void init(TLiveManager *) override;
  virtual void control() override;
  virtual void bind() override;
  virtual void moveObject() override;
  virtual void *getBasNameTable() const;

  f32 getAngleToTarget() const;

  bool isFollowMario() const { return mIsFollowMario; }
  bool isPounding() const { return mIsPounding; }
  bool isGooping() const { return mIsGooping; }
  bool isShocking() const { return mIsShocking; }
  bool isTargetInRangeToHome(f32 range) const;

  PoundingState advanceDropAttack(TPollutionManager *gpPollution,
                                  TMario *player);
  void advanceGoopDroplet();
  void advanceRollMovement(TPollutionManager *gpPollution);
  void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics,
                           TDarkZhine *thisZhine);
  static u32 cleanFromSpineBase(u32 *gpNerveBGEye,
                                TSpineBase<TLiveActor> *gpSpineBase);
};

#endif