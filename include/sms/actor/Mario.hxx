#pragma once

#include "J3D/J3DDrawBuffer.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JGeometry.hxx"
#include "JUT/JUTGamePad.hxx"
#include "MTX.h"
#include "types.h"

#include "sms/actor/TakeActor.hxx"
#include "sms/actor/Yoshi.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/m3d/M3UModel.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/params/BaseParam.hxx"
#include "sms/params/Params.hxx"

#include "macros.h"

#define MESSAGE_MARIO_TOSS 7
#define MESSAGE_MARIO_BURN 10

#define OBJECT_ID_MARIO 0x80000001

class TMario;

class TDrawSyncCallback {
public:
  virtual void drawSyncCallback(u16) = 0;
};

struct TMarioAnimeData {
  enum FLUDD { FLUDD_ENABLED = 68, FLUDD_DISABLED = 200 };

  enum HAND { HAND_A, HAND_B, HAND_C };

  bool isPumpOK() const;

  u16 mAnimID;
  u16 mFluddEnabled; // Note: ENUM? 68 = enabled, 200 = disabled
  u8 mAnmTexPattern; // Note: 23 is the max value allowed
  u8 mMarioHand; // Note: 1, 2, and 3 seem to be valid values, this determines
                 // the hand model used(?)
  u8 unk_1;      // Note: If bit 30 is set, it seems to activate a bit flag to
                 // multiple pointers, likely mtx related
  u8 unk_2;      // Note: Value seems to be set but is never used?
};

class TMarioCap {
public:
  TMarioCap(TMario *);

  virtual void perform(u32, JDrama::TGraphics *);

  void createMirrorModel();
  void mtxEffectHide();
  void mtxEffectShow();
};

class M3UModelMario : public M3UModel {
public:
  virtual void updateIn() override;
  virtual void updateOut() override;

  void changeMtxCalcSIAnmBQAnmTransform(int, int, u8);
  void updateInMotion();
};

class TMarioSoundValues {
public:
  TMarioSoundValues();

  u32 _0;
  u32 _4;
  u32 _8;
  u32 _C;
  u32 _10;
  u32 _14;
  u8 _18;
  u32 _1C;
  u8 _20;
  u16 _22;
  u8 _24;
  u16 _26;
  u16 _28;
  u8 _2A;
  u8 _2B;
  u8 _2C;
};

struct TMarioControllerWork {
  enum Buttons { R = 0x20, A = 0x100, B = 0x200 };

  s16 mStickHS16;
  s16 mStickVS16;
  Buttons mInput;
  Buttons mFrameInput;
  u8 mAnalogRU8;
  u8 mAnalogLU8;
  f32 mStickH;
  f32 mStickV;
  f32 mStickDist;
  f32 mAnalogL;
  f32 mAnalogR;
};

class TMarioGamePad : public JUTGamePad {
public:
  virtual ~TMarioGamePad();

  void onNeutralMarioKey();
  void read();
  void reset();
  void updateMeaning();

  u32 _A4;
  f32 _A8;
  f32 _AC;
  f32 _B0;
  f32 _B4;
  f32 _B8;
  f32 _BC;
  f32 mStickX; // _C0
  f32 mStickY; // _C4
  f32 _C8;
  f32 _CC;
  u32 mMeaning;
  u32 _D4;
  u32 _D8;
  u16 _DC;
  u16 _DE;
  u16 _E0;

  struct {
    u16 _00 : 8;
    bool mDisable : 1;
    u8 _01 : 5;
    bool mReadInput : 1;
    u8 _02 : 1;
  } State; // 0x00E2

  u16 _E4;
  u16 _E6; // padding?
  u32 _E8;
  u32 _EC; // padding?
};

class TMario : public TTakeActor, public TDrawSyncCallback {
public:
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  struct TDeParams : public TParams {
    TDeParams();

    TParamT<s16> mHPMax;           // Max health
    TParamT<f32> mRunningMax;      // Max running speed
    TParamT<f32> mDashMax;         // Turbo Nozzle max speed
    TParamT<f32> mDashAcc;         // Turbo Nozzle acceleration
    TParamT<f32> mDashBrake;       // Turbo Nozzle stop speed?
    TParamT<f32> mDashStartTime;   // Turbo Nozzle start
    TParamT<f32> mWaitingRotSp;    // Waiting rotation speed
    TParamT<f32> mRunningRotSpMin; // Running rotation speed min
    TParamT<f32> mRunningRotSpMax; // Running rotation speed max
    TParamT<f32> mRocketRotSp;     // Rocket Nozzle rotation speed
    TParamT<f32> mPumpingRotSpMin; // Pumping rotation speed min
    TParamT<f32> mPumpingRotSpMax; // Pumping rotation speed max
    TParamT<f32> mInvincibleTime;
    TParamT<s16> mFootPrintTimerMax;
    TParamT<f32> mWaterTriggerRate;
    TParamT<f32> mGraffitoNoDmgTime;
    TParamT<f32> mRestMax;
    TParamT<f32> mShadowSize;
    TParamT<f32> mShadowErase;
    TParamT<f32> mHoldRadius;
    TParamT<f32> mDamageRadius;
    TParamT<f32> mDamageHeight;
    TParamT<f32> mAttackHeight;
    TParamT<f32> mTrampleRadius;
    TParamT<f32> mPushupRadius;
    TParamT<f32> mPushupHeight;
    TParamT<f32> mHipdropRadius;
    TParamT<f32> mQuakeRadius;
    TParamT<f32> mQuakeHeight;
    TParamT<f32> mTramplePowStep1;
    TParamT<f32> mTramplePowStep2;
    TParamT<f32> mTramplePowStep3;
    TParamT<f32> mJumpWallHeight;
    TParamT<f32> mThrowPower;
    TParamT<f32> mSlipStart;
    TParamT<f32> mWasOnWaterSlip;
    TParamT<f32> mInWaterSlip;
    TParamT<f32> mToroccoRotSp;
    TParamT<s16> mRecoverTimer;
    TParamT<s16> mHotTimer;
    TParamT<f32> mFeelDeep;
    TParamT<f32> mDamageFallHeight;
    TParamT<f32> mForceSlipAngle;
    TParamT<f32> mClashSpeed; // Wall bonk speed?
    TParamT<f32> mHangWallMovableAngle;
    TParamT<f32> mColMvMax;
    TParamT<s16> mNoFreezeTime;   // Wall hang no freeze timer
    TParamT<s16> mKickFreezeTime; // Wall hang freeze timer
    TParamT<s16> mSurfStartFreezeTime;
    TParamT<f32> mSleepingCheckDist;
    TParamT<f32> mSleepingCheckHeight;
    TParamT<s16> mIllegalPlaneCtInc; // Death plane timer increment
    TParamT<s16> mIllegalPlaneTime;  // Death plane timer max
  };

  struct TBodyAngleParams : public TParams {
    TBodyAngleParams(const char *prm)
        : CONSTRUCT_PARAM(mHeadRot, 0.0f), CONSTRUCT_PARAM(mWaistRoll, 0.0f),
          CONSTRUCT_PARAM(mWaistPitch, 80.0f),
          CONSTRUCT_PARAM(mWaistRollMax, 0),
          CONSTRUCT_PARAM(mWaistPitchMax, 1000),
          CONSTRUCT_PARAM(mWaistAngleChangeRate, 0.07f) {
      load(prm);
    }

    TParamT<f32> mHeadRot;
    TParamT<f32> mWaistRoll;
    TParamT<f32> mWaistPitch;
    TParamT<s16> mWaistRollMax;
    TParamT<s16> mWaistPitchMax;
    TParamT<f32> mWaistAngleChangeRate;
  };

  struct TAttackParams : public TParams {
    TAttackParams(const char *prm)
        : CONSTRUCT_PARAM(mRadius, 100.0f), CONSTRUCT_PARAM(mHeight, 50.0f) {
      load(prm);
    }

    TParamT<f32> mRadius;
    TParamT<f32> mHeight;
  };

  struct TJumpParams : public TParams {
    TJumpParams();

    TParamT<f32> mGravity;
    TParamT<f32> mSpinJumpGravity;
    TParamT<f32> mJumpingMax;
    TParamT<f32> mJumpSpeedBrake;
    TParamT<f32> mJumpSpeedAccelControl;
    TParamT<f32> mJumpSlideControl;
    TParamT<f32> mTurnJumpForce;
    TParamT<f32> mFenceSpeed;
    TParamT<f32> mFireDownForce;
    TParamT<f32> mFireDownControl;
    TParamT<f32> mFireBackVelocity;
    TParamT<f32> mBroadJumpForce;
    TParamT<f32> mBroadJumpForceY;
    TParamT<f32> mRotateJumpForceY;
    TParamT<f32> mPopUpSpeedY;
    TParamT<f32> mPopUpForceYMult;
    TParamT<f32> mBackJumpForce;
    TParamT<f32> mBackJumpForceY;
    TParamT<f32> mHipAttackSpeedY;
    TParamT<f32> mSuperHipAttackSpeedY;
    TParamT<f32> mJumpCatchRotXSp;
    TParamT<f32> mJumpCatchRotXMax;
    TParamT<f32> mRotBroadEnableV;
    TParamT<f32> mRotBroadJumpForce;
    TParamT<f32> mRotBroadJumpForceY;
    TParamT<f32> mTrampolineDec;
    TParamT<f32> mSecJumpEnableSp;
    TParamT<f32> mSecJumpForce;
    TParamT<f32> mSecJumpSpeedMult;
    TParamT<f32> mSecJumpXZMult;
    TParamT<f32> mTriJumpEnableSp;
    TParamT<f32> mUltraJumpForce;
    TParamT<f32> mUltraJumpSpeedMult;
    TParamT<f32> mUltraJumpXZMult;
    TParamT<f32> mValleyDepth;
    TParamT<f32> mThrownAccel;
    TParamT<f32> mThrownSlide;
    TParamT<f32> mThrownBrake;
    TParamT<f32> mTremblePower;
    TParamT<f32> mTrembleAccele;
    TParamT<f32> mTrembleBrake;
    TParamT<s16> mTrembleTime;
    TParamT<s16> mClashAngle;
    TParamT<f32> mJumpJumpCatchSp;
    TParamT<f32> mGetOffYoshiY;
    TParamT<s16> mSuperHipAttackCt; // Time until super hip attack?
  };

  struct TRunParams : public TParams {
    TRunParams();

    TParamT<f32> mMaxSpeed;
    TParamT<f32> mVelMinusBrake;
    TParamT<f32> mAddBase;
    TParamT<f32> mAddVelDiv;
    TParamT<f32> mDecStartNrmY;
    TParamT<f32> mDecBrake;
    TParamT<f32> mSoft2Walk;
    TParamT<f32> mWalk2Soft;
    TParamT<f32> mSoftStepAnmMult;
    TParamT<f32> mRunAnmSpeedBase;
    TParamT<f32> mRunAnmSpeedMult;
    TParamT<f32> mMotBlendWalkSp;
    TParamT<f32> mMotBlendRunSp;
    TParamT<f32> mSwimDepth;
    TParamT<f32> mInWaterBrake;
    TParamT<f32> mInWaterAnmBrake;
    TParamT<f32> mPumpingSlideSp;
    TParamT<f32> mPumpingSlideAnmSp;
    TParamT<f32> mDoJumpCatchSp;
    TParamT<f32> mTurnNeedSp;
    TParamT<s16> mDashRotSp;
  };

  struct TSwimParams : public TParams {
    TSwimParams();

    TParamT<f32> mStartSp;
    TParamT<f32> mMoveSp;
    TParamT<f32> mMoveBrake;
    TParamT<s16> mSwimmingRotSpMin;
    TParamT<s16> mSwimmingRotSpMax;
    TParamT<s16> mPumpingRotSpMin;
    TParamT<s16> mPumpingRotSpMax;
    TParamT<f32> mGravity;
    TParamT<f32> mWaitBouyancy;
    TParamT<f32> mMoveBouyancy;
    TParamT<f32> mUpDownBrake;
    TParamT<f32> mCanJumpDepth;
    TParamT<f32> mEndDepth;
    TParamT<f32> mFloatHeight;
    TParamT<f32> mStartVMult;
    TParamT<f32> mStartVYMult;
    TParamT<f32> mRush;
    TParamT<f32> mAnmBrake;
    TParamT<f32> mPaddleSpeedUp;
    TParamT<f32> mPaddleJumpUp;
    TParamT<f32> mFloatUp;
    TParamT<f32> mWaterLevelCheckHeight;
    TParamT<f32> mPaddleDown;
    TParamT<s16> mWaitSinkTime;
    TParamT<f32> mCanBreathDepth;
    TParamT<f32> mWaitSinkSpeed;
    TParamT<f32> mAirDec;
    TParamT<f32> mAirDecDive;
    TParamT<f32> mAirInc;
  };

  struct THangFenceParams : public TParams {
    THangFenceParams()
        : CONSTRUCT_PARAM(mMoveSp, 0.1f), CONSTRUCT_PARAM(mAnmRate, 0.5f),
          CONSTRUCT_PARAM(mRapidTime, 2000), CONSTRUCT_PARAM(mLimitTime, 2400),
          CONSTRUCT_PARAM(mAnmRapid, 8.0f), CONSTRUCT_PARAM(mDescentSp, 10.0f) {
      load("/Mario/Hanging.prm");
    }

    TParamT<f32> mMoveSp;
    TParamT<f32> mAnmRate;
    TParamT<s16> mRapidTime;
    TParamT<s16> mLimitTime;
    TParamT<f32> mAnmRapid;
    TParamT<f32> mDescentSp;
  };

  struct THangRoofParams : public TParams {
    THangRoofParams() : CONSTRUCT_PARAM(mAnmMult, 0.3f) {
      load("/Mario/HangRoof.prm");
    }

    TParamT<f32> mAnmMult;
  };

  struct TWireParams : public TParams {
    TWireParams();

    TParamT<s16> mRotSpeed;
    TParamT<s16> mRotSpeedTrgHover;
    TParamT<s16> mRotSpeedTrgTurbo;
    TParamT<s16> mRotSpeedTrgRocket;
    TParamT<s16> mRotSpeedMax;
    TParamT<s16> mRotStop;
    TParamT<s16> mRotGravity;
    TParamT<f32> mRotBrake;
    TParamT<f32> mJumpRate;
    TParamT<f32> mSwingRate;
    TParamT<f32> mWireJumpAccelControl;
    TParamT<f32> mWireJumpSlideControl;
    TParamT<f32> mWireJumpMult;
    TParamT<f32> mWireJumpBase;
    TParamT<f32> mWireSwingBrake;
    TParamT<f32> mWireSwingMax;
  };

  struct TPullParams : public TParams {
    TPullParams(const char *prm)
        : CONSTRUCT_PARAM(mPullRateV, 0.3f), CONSTRUCT_PARAM(mPullRateH, 0.05f),
          CONSTRUCT_PARAM(mOilPullRateV, 0.1f),
          CONSTRUCT_PARAM(mOilPullRateH, 0.01f) {
      load(prm);
    }

    TParamT<f32> mPullRateV;
    TParamT<f32> mPullRateH;
    TParamT<f32> mOilPullRateV;
    TParamT<f32> mOilPullRateH;
  };

  struct TClimbParams : public TParams {
    TClimbParams()
        : CONSTRUCT_PARAM(mClimbSp, 0.035f), CONSTRUCT_PARAM(mRotateSp, 3.0f),
          CONSTRUCT_PARAM(mClimbAnmRate, 1.0f / 256.0f),
          CONSTRUCT_PARAM(mCatchRadius, 100.0f),
          CONSTRUCT_PARAM(mCatchAngle, 0.8f) {
      load("/Mario/Bar.prm");
    }

    TParamT<f32> mClimbSp;
    TParamT<f32> mRotateSp;
    TParamT<f32> mClimbAnmRate;
    TParamT<f32> mCatchRadius;
    TParamT<f32> mCatchAngle;
  };

  struct TSurfingParams : public TParams {
    TSurfingParams(const char *prm);

    TParamT<f32> mRotMin;
    TParamT<f32> mRotMax;
    TParamT<f32> mPowMin;
    TParamT<f32> mPowMax;
    TParamT<f32> mAccel;
    TParamT<f32> mWaistRoll;
    TParamT<f32> mWaistPitch;
    TParamT<s16> mWaistRollMax;
    TParamT<s16> mWaistPitchMax;
    TParamT<f32> mRoll;
    TParamT<f32> mPitch;
    TParamT<s16> mRollMax;
    TParamT<s16> mPitchMax;
    TParamT<f32> mAngleChangeRate;
    TParamT<f32> mWaistAngleChangeRate;
    TParamT<f32> mScaleMin;
    TParamT<f32> mScaleMax;
    TParamT<f32> mScaleMinSpeed;
    TParamT<f32> mScaleMaxSpeed;
    TParamT<f32> mJumpPow;
    TParamT<f32> mJumpXZRatio;
    TParamT<f32> mClashSpeed;
    TParamT<s16> mClashAngle;
  };

  struct THoverParams : public TParams {
    THoverParams()
        : CONSTRUCT_PARAM(mRotSp, 128), CONSTRUCT_PARAM(mAccelRate, 0.03f),
          CONSTRUCT_PARAM(mBrake, 0.95f) {
      load("/Mario/HHover.prm");
    }

    TParamT<s16> mRotSp;
    TParamT<f32> mAccelRate;
    TParamT<f32> mBrake;
  };

  struct TDivingParams : public TParams {
    TDivingParams()
        : CONSTRUCT_PARAM(mRotSp, 128), CONSTRUCT_PARAM(mGravity, 0.5f),
          CONSTRUCT_PARAM(mAccelControl, 0.02f),
          CONSTRUCT_PARAM(mSeaBrake, 0.999f),
          CONSTRUCT_PARAM(mSeaBrakeY, 0.98f) {
      load("/Mario/Diving.prm");
    }

    TParamT<s16> mRotSp;
    TParamT<f32> mGravity;
    TParamT<f32> mAccelControl;
    TParamT<f32> mSeaBrake;
    TParamT<f32> mSeaBrakeY;
  };

  struct TYoshiParams : public TParams {
    TYoshiParams()
        : CONSTRUCT_PARAM(mRunYoshiMult, 1.2f),
          CONSTRUCT_PARAM(mJumpYoshiMult, 1.0f),
          CONSTRUCT_PARAM(mRotYoshiMult, 1.5f),
          CONSTRUCT_PARAM(mHeadFront, 80.0f),
          CONSTRUCT_PARAM(mHeadRadius, 50.0f),
          CONSTRUCT_PARAM(mHoldOutAccCtrlF, 0.01f),
          CONSTRUCT_PARAM(mHoldOutAccCtrlB, 0.023f),
          CONSTRUCT_PARAM(mHoldOutSldCtrl, 0.3f),
          CONSTRUCT_PARAM(mDecBrake, 1.0f) {
      load("/Mario/Yoshi.prm");
    }

    TParamT<f32> mRunYoshiMult;
    TParamT<f32> mJumpYoshiMult;
    TParamT<f32> mRotYoshiMult;
    TParamT<f32> mHeadFront;
    TParamT<f32> mHeadRadius;
    TParamT<f32> mHoldOutAccCtrlF;
    TParamT<f32> mHoldOutAccCtrlB;
    TParamT<f32> mHoldOutSldCtrl;
    TParamT<f32> mDecBrake;
  };

  struct TWaterEffectParams : public TParams {
    TWaterEffectParams()
        : CONSTRUCT_PARAM(mJumpIntoMdlEffectSpY, 10.0f),
          CONSTRUCT_PARAM(mJumpIntoMinY, 20.0f),
          CONSTRUCT_PARAM(mJumpIntoMaxY, 50.0f),
          CONSTRUCT_PARAM(mJumpIntoScaleMin, 0.75f),
          CONSTRUCT_PARAM(mJumpIntoScaleWidth, 1.0f),
          CONSTRUCT_PARAM(mRunningRippleSpeed, 30.0f),
          CONSTRUCT_PARAM(mRunningRippleDepth, 30.0f) {
      load("/Mario/WaterEffect.prm");
    }

    TParamT<f32> mJumpIntoMdlEffectSpY;
    TParamT<f32> mJumpIntoMinY;
    TParamT<f32> mJumpIntoMaxY;
    TParamT<f32> mJumpIntoScaleMin;
    TParamT<f32> mJumpIntoScaleWidth;
    TParamT<f32> mRunningRippleSpeed;
    TParamT<f32> mRunningRippleDepth;
  };

  struct TControllerParams : public TParams {
    TControllerParams()
        : CONSTRUCT_PARAM(mAnalogLRToZeroVal, 30),
          CONSTRUCT_PARAM(mAnalogLRToMiddleVal, 90),
          CONSTRUCT_PARAM(mAnalogLRToMaxVal, 150),
          CONSTRUCT_PARAM(mAnalogLRMiddleVal, 0.1f),
          CONSTRUCT_PARAM(mStartToWalkLevel, 15.0f),
          CONSTRUCT_PARAM(mStickRotateTime, 24),
          CONSTRUCT_PARAM(mLengthMultTimes, 10),
          CONSTRUCT_PARAM(mLengthMult, 0.935f),
          CONSTRUCT_PARAM(mSquatRotMidAnalog, 0.7f),
          CONSTRUCT_PARAM(mSquatRotMidValue, 0.05f) {
      load("/Mario/Controller.prm");
    }

    TParamT<u8> mAnalogLRToZeroVal;
    TParamT<u8> mAnalogLRToMiddleVal;
    TParamT<u8> mAnalogLRToMaxVal;
    TParamT<f32> mAnalogLRMiddleVal;
    TParamT<f32> mStartToWalkLevel;
    TParamT<s16> mStickRotateTime;
    TParamT<s16> mLengthMultTimes;
    TParamT<f32> mLengthMult;
    TParamT<f32> mSquatRotMidAnalog;
    TParamT<f32> mSquatRotMidValue;
  };

  struct TGraffitoParams : public TParams {
    TGraffitoParams();

    TParamT<s16> mSinkTime;
    TParamT<s16> mSinkDmgTime;
    TParamT<f32> mSinkHeight;
    TParamT<f32> mSinkMoveMin;
    TParamT<f32> mSinkMoveMax;
    TParamT<f32> mSinkRecover;
    TParamT<f32> mSinkJumpRateMin;
    TParamT<f32> mSinkJumpRateMax;
    TParamT<f32> mSinkPumpLimit;
    TParamT<f32> mSinkDmgDepth;
    TParamT<f32> mFireHeight;
    TParamT<s16> mDizzySlipCtMax;
    TParamT<s16> mDizzyWalkCtMax;
    TParamT<s16> mDizzyAngleY;
    TParamT<f32> mDizzyAngleRate;
    TParamT<f32> mDizzyPowerRate;
    TParamT<f32> mDizzyPower;
    TParamT<s16> mFireInvincibleTime;
    TParamT<s16> mFootEraseTimes;
    TParamT<f32> mFootEraseSize;
    TParamT<f32> mFootEraseFront;
  };

  struct TDirtyParams : public TParams {
    TDirtyParams()
        : CONSTRUCT_PARAM(mIncRunning, 0.1f),
          CONSTRUCT_PARAM(mIncCatching, 0.3f),
          CONSTRUCT_PARAM(mIncSlipping, 0.2f),
          CONSTRUCT_PARAM(mDecSwimming, 0.5f),
          CONSTRUCT_PARAM(mDecWaterHit, 0.2f),
          CONSTRUCT_PARAM(mDecRotJump, 0.1f),
          CONSTRUCT_PARAM(mBrakeStartValSlip, 0.99f),
          CONSTRUCT_PARAM(mBrakeStartValRun, 0.98f),
          CONSTRUCT_PARAM(mDirtyTimeSlip, 600),
          CONSTRUCT_PARAM(mDirtyTimeRun, 600),
          CONSTRUCT_PARAM(mPolSizeSlip, 200.0f),
          CONSTRUCT_PARAM(mPolSizeRun, 80.0f),
          CONSTRUCT_PARAM(mPolSizeFootPrint, 200.0f),
          CONSTRUCT_PARAM(mPolSizeJump, 200.0f),
          CONSTRUCT_PARAM(mSlopeAngle, 0.99f),
          CONSTRUCT_PARAM(mDirtyMax, 200.0f),
          CONSTRUCT_PARAM(mSlipAnmSpeed, 3.0f),
          CONSTRUCT_PARAM(mSlipRunSp, 0.01f),
          CONSTRUCT_PARAM(mSlipCatchSp, 0.01f),
          CONSTRUCT_PARAM(mSlipRotate, 100),
          CONSTRUCT_PARAM(mSlipCatchRotate, 100),
          CONSTRUCT_PARAM(mBrakeSlipNoPollute, 0.98f),
          CONSTRUCT_PARAM(mFogTimeYellow, 240),
          CONSTRUCT_PARAM(mFogTimeRed, 600) {
      load("/Mario/Dirty.prm");
    }

    TParamT<f32> mIncRunning;
    TParamT<f32> mIncCatching;
    TParamT<f32> mIncSlipping;
    TParamT<f32> mDecSwimming;
    TParamT<f32> mDecWaterHit;
    TParamT<f32> mDecRotJump;
    TParamT<f32> mBrakeStartValSlip;
    TParamT<f32> mBrakeStartValRun;
    TParamT<s16> mDirtyTimeSlip;
    TParamT<s16> mDirtyTimeRun;
    TParamT<f32> mPolSizeSlip;
    TParamT<f32> mPolSizeRun;
    TParamT<f32> mPolSizeFootPrint;
    TParamT<f32> mPolSizeJump;
    TParamT<f32> mSlopeAngle;
    TParamT<f32> mDirtyMax;
    TParamT<f32> mSlipAnmSpeed;
    TParamT<f32> mSlipRunSp;
    TParamT<f32> mSlipCatchSp;
    TParamT<s16> mSlipRotate;
    TParamT<s16> mSlipCatchRotate;
    TParamT<f32> mBrakeSlipNoPollute;
    TParamT<s16> mFogTimeYellow;
    TParamT<s16> mFogTimeRed;
  };

  struct TMotorParams : public TParams {
    TMotorParams()
        : CONSTRUCT_PARAM(mMotorReturn, 25), CONSTRUCT_PARAM(mMotorTrample, 8),
          CONSTRUCT_PARAM(mMotorHipDrop, 15), CONSTRUCT_PARAM(mMotorWall, 6) {
      load("/Mario/MarioMotor.prm");
    }

    TParamT<s16> mMotorReturn;
    TParamT<s16> mMotorTrample;
    TParamT<s16> mMotorHipDrop;
    TParamT<s16> mMotorWall;
  };

  struct TParticleParams : public TParams {
    TParticleParams()
        : CONSTRUCT_PARAM(mMeltInWaterMax, 0.5f),
          CONSTRUCT_PARAM(mWaveEmitSpeed, 5.0f),
          CONSTRUCT_PARAM(mWaveAlphaDec, 5),
          CONSTRUCT_PARAM(mBubbleDepth, 10.0f),
          CONSTRUCT_PARAM(mBodyBubbleSpMin, 0.0f),
          CONSTRUCT_PARAM(mBodyBubbleSpMax, 40.0f),
          CONSTRUCT_PARAM(mBodyBubbleEmitMin, 0.0f),
          CONSTRUCT_PARAM(mBodyBubbleEmitMax, 0.5f),
          CONSTRUCT_PARAM(mBubbleToRipple, 0.3f),
          CONSTRUCT_PARAM(mToroccoWind, 0.001f),
          CONSTRUCT_PARAM(mToroccoSpark, 0.001f) {
      load("/Mario/MarioParticle.prm");
    }

    TParamT<f32> mMeltInWaterMax;
    TParamT<f32> mWaveEmitSpeed;
    TParamT<s16> mWaveAlphaDec;
    TParamT<f32> mBubbleDepth;
    TParamT<f32> mBodyBubbleSpMin;
    TParamT<f32> mBodyBubbleSpMax;
    TParamT<f32> mBodyBubbleEmitMin;
    TParamT<f32> mBodyBubbleEmitMax;
    TParamT<f32> mBubbleToRipple;
    TParamT<f32> mToroccoWind;
    TParamT<f32> mToroccoSpark;
  };

  struct TEffectParams : public TParams {
    TEffectParams()
        : CONSTRUCT_PARAM(mDashInc, 1.0f / 3.0f),
          CONSTRUCT_PARAM(mDashDec, 1.0f / 6.0f),
          CONSTRUCT_PARAM(mDashMaxBlendInBlur, 180),
          CONSTRUCT_PARAM(mDashMaxBlendInIris, 180),
          CONSTRUCT_PARAM(mDashBlendScale, 0.2f) {
      load("/Mario/MarioEffect.prm");
    }

    TParamT<f32> mDashInc;
    TParamT<f32> mDashDec;
    TParamT<u8> mDashMaxBlendInBlur;
    TParamT<u8> mDashMaxBlendInIris;
    TParamT<f32> mDashBlendScale;
  };

  struct TSlipParams : public TParams {
    TSlipParams(const char *prm)
        : CONSTRUCT_PARAM(mSlipFriction, 0.9f),
          CONSTRUCT_PARAM(mSlopeAcceleUp, 0.0f),
          CONSTRUCT_PARAM(mSlopeAcceleDown, 0.0f),
          CONSTRUCT_PARAM(mSlideAcceleUp, 0.0f),
          CONSTRUCT_PARAM(mSlideAcceleDown, 0.0f),
          CONSTRUCT_PARAM(mSlideStopNormal, 15.0f),
          CONSTRUCT_PARAM(mSlideStopCatch, 15.0f),
          CONSTRUCT_PARAM(mJumpEnable, 1), CONSTRUCT_PARAM(mMissJump, 1),
          CONSTRUCT_PARAM(mSlideAngleYSp, 512),
          CONSTRUCT_PARAM(mStickSlideMult, 0.05) {
      load(prm);
    }

    TParamT<f32> mSlipFriction;
    TParamT<f32> mSlopeAcceleUp;
    TParamT<f32> mSlopeAcceleDown;
    TParamT<f32> mSlideAcceleUp;
    TParamT<f32> mSlideAcceleDown;
    TParamT<f32> mSlideStopNormal;
    TParamT<f32> mSlideStopCatch;
    TParamT<u8> mJumpEnable;
    TParamT<u8> mMissJump;
    TParamT<s16> mSlideAngleYSp;
    TParamT<f32> mStickSlideMult;
  };

  struct TUpperBodyParams : public TParams {
    TUpperBodyParams()
        : CONSTRUCT_PARAM(mPumpWaitTime, 10),
          CONSTRUCT_PARAM(mPumpAnmSpeed, 0.01f),
          CONSTRUCT_PARAM(mHoverHeadAngle, -8192),
          CONSTRUCT_PARAM(mFeelDeepHeadAngle, 8192),
          CONSTRUCT_PARAM(mFrontWallHeadAngle, -8192) {
      load("/Mario/UpperBody.prm");
    }

    TParamT<s16> mPumpWaitTime;
    TParamT<f32> mPumpAnmSpeed;
    TParamT<s16> mHoverHeadAngle;
    TParamT<s16> mFeelDeepHeadAngle;
    TParamT<s16> mFrontWallHeadAngle;
  };

  struct TDmgEnemyParams : public TParams {
    TDmgEnemyParams(const char *prm)
        : CONSTRUCT_PARAM(mDamage, 1), CONSTRUCT_PARAM(mDownType, 1),
          CONSTRUCT_PARAM(mWaterEmit, 0), CONSTRUCT_PARAM(mMotor, 25),
          CONSTRUCT_PARAM(mMinSpeed, 16.0f), CONSTRUCT_PARAM(mDirty, 0.0f),
          CONSTRUCT_PARAM(mInvincibleTime, 300) {
      load(prm);
    }

    TParamT<u8> mDamage;
    TParamT<u8> mDownType;
    TParamT<u8> mWaterEmit;
    TParamT<u8> mMotor;
    TParamT<f32> mMinSpeed;
    TParamT<f32> mDirty;
    TParamT<s16> mInvincibleTime;
  };

  struct TDemoParams : public TParams {
    TDemoParams()
        : CONSTRUCT_PARAM(mWarpInBallsDispTime, 6),
          CONSTRUCT_PARAM(mWarpInBallsTime, 70),
          CONSTRUCT_PARAM(mWarpInCapturedTime, 120),
          CONSTRUCT_PARAM(mWarpInTremble, 15.0f),
          CONSTRUCT_PARAM(mWarpInVecBase, 0.3f),
          CONSTRUCT_PARAM(mWarpTransTremble, 50.0f),
          CONSTRUCT_PARAM(mReadRotSp, 1024) {
      load("/Mario/AutoDemo.prm");
    }

    TParamT<s16> mWarpInBallsDispTime;
    TParamT<s16> mWarpInBallsTime;
    TParamT<s16> mWarpInCapturedTime;
    TParamT<f32> mWarpInTremble;
    TParamT<f32> mWarpInVecBase;
    TParamT<f32> mWarpTransTremble;
    TParamT<s16> mReadRotSp;
  };

  struct TSoundParams : public TParams {
    TSoundParams() : CONSTRUCT_PARAM(mStartFallVoiceSpeed, 60.0f) {
      load("/Mario/Sound.prm");
    }

    TParamT<f32> mStartFallVoiceSpeed;
  };

  struct TOptionParams : public TParams {
    TOptionParams()
        : CONSTRUCT_PARAM(mZ, -1000.0f), CONSTRUCT_PARAM(mXMin, 846.0f),
          CONSTRUCT_PARAM(mXMax, 2000.0f) {
      load("/Mario/Option.prm");
    }

    TParamT<f32> mZ;
    TParamT<f32> mXMin;
    TParamT<f32> mXMax;
  };

#undef CONSTRUCT_PARAM

  enum State : u32 {
    STATE_NUMBER = 0x0000000F,
    STATE_DOJUMP = 0x00000080,
    STATE_AIRBORN = 0x00000800,
    STATE_CUTSCENE = 0x00001000,
    STATE_WATERBORN = 0x00002000,
    STATE_RUNNING = 0x04000440,
    STATE_IDLE = 0x0C400201,
    STATE_STOP = 0x0C00023D,
    STATE_SPIN = 0x00000441,
    STATE_JUMPSPIN = 0x00000890,
    STATE_JUMPSPINR = 0x00000895,
    STATE_JUMPSPINL = 0x00000896,
    STATE_JUMP = 0x02000880,
    STATE_SLIP_JUMP = 0x02000885,
    STATE_D_JUMP = 0x02000881,
    STATE_TRIPLE_J = 0x00000882,
    STATE_JMP_LAND = 0x04000470,
    STATE_HVY_LAND = 0x04000473,
    STATE_D_LAND = 0x04000472,
    STATE_T_LAND = 0x0800023A,
    STATE_JUMPSIDE = 0x00000880,
    STATE_BOUNCE = 0x00000884,
    STATE_SIDESTEP = 0x0C008220,
    STATE_SIDE_FLIP = 0x00000887,
    STATE_FALL = 0x0000088C,
    STATE_SWIM = 0x000024D7,
    STATE_DIVE = 0x0080088A,
    STATE_DIVEJUMP = 0x02000889,
    STATE_DIVESLIDE = 0x00800456,
    STATE_GOOPSLIDE = 0x0084045D,
    STATE_CLIMB = 0x18100340,
    STATE_CLIMBUP = 0x10100343,
    STATE_WALLJUMP = 0x02000886,
    STATE_WALLSLIDE = 0x000008A7,
    STATE_ROOFHANG = 0x00200349,
    STATE_HANG = 0x3800034B,
    STATE_HANGCLIMB = 0x3000054F,
    STATE_SLAMSTART = 0x008008A9,
    STATE_SLAM = 0x0080023C,
    STATE_SPRAY = 0x0C008220,
    STATE_THROWN = 0x000208B8,
    STATE_HOVER = 0x0000088B,
    STATE_STUCK = 0x0002033C,
    STATE_TALKING = 0x10001308,
    STATE_TURNING_MID = 0x00000443,
    STATE_TURNING = 0x00000444,
    STATE_YOSHI_ESC = 0x0000089C,
    STATE_SHINE_C = 0x00001302, // Collected Shine Sprite
    STATE_DEATH = 0x00020467,
    STATE_DOOR_F_O = 0x00001321, // Door open fail
    STATE_WALL_S_L = 0x04000471,
    STATE_F_KNCK_H = 0x000208B0, // hard knockback from front (bumping wall from dive)
    STATE_KNCK_LND = 0x00020462, // Landing from front knockback
    STATE_KNCK_GND = 0x00020466, // Front knockback while grounded
    STATE_FIRE_HIT = 0x000208B7,
    STATE_FIRE_RVR = 0x08000239, // Recover from fire on ground
    STATE_HOVER_F = 0x0000088D,  // Falling from hover
    STATE_SLIDE_R1 = 0x000008A6, // Recover from slide by flipping
    STATE_SLIDE_R2 = 0x00000386, // Recover from slide by getting up
    STATE_R_SPRAY = 0x0C008220,  // Recover from spray
    STATE_G_POUND = 0x008008A9,  // Ground pounding
    STATE_NPC_PUTDOWN = 0x80000387,
    STATE_NPC_THROW = 0x80000588,
    STATE_NPC_JUMPTHROW = 0x820008AB
  };

  enum Status : u32 {
    STATUS_ALIVE = 0x1,
    STATUS_SEWER_FLOOR = 0x2,
    STATUS_INVISIBLE = 0x4,
    STATUS_NPC_TALKING = 0x8,
    STATUS_LEAVING_WATER = 0x10,
    STATUS_SLIP = 0x80,
    STATUS_GAMEOVER = 0x400,
    STATUS_UP_GROUND_POUND = 0x800,
    STATUS_HASHELMET_FOLLOWCAMERA = 0x1000,
    STATUS_HASHELMET = 0x2000,
    STATUS_HASFLUDD = 0x8000,
    STATUS_SPLASH = 0x10000,
    STATUS_PUDDLE = 0x20000,
    STATUS_SHIRT = 0x100000,
    STATUS_GONE = 0x200000
  };

  enum Voice : u32 {
    VOICE_CANNON_WAIL = 30911,
    VOICE_TRIPLE_JUMP = 30902,
    VOICE_JUMP = 30891,
    VOICE_DIVE_OUT = 30897,
    VOICE_FALL_LEDGE_GRAB = 30944
  };

  enum Animation : u32 {
    ANIMATION_WALLHANG = 0x33,
    ANIMATION_FALL = 0x4C,
    ANIMATION_BOUNCE = 0x50,
    ANIMATION_IDLE = 0xC3,
    ANIMATION_SHINEGET = 0xCD,
    ANIMATION_SPINJUMP = 0xF4
  };

  enum Effect : u32 {
    EFFECT_SMOKE_CLOUD = 0x1,
    EFFECT_ROCKET_SPRAY_MIST = 0x2,
    EFFECT_ROCKET_SPRAY_DROPS = 0x3,
    EFFECT_BURNING_SMOKE = 0x6,
    EFFECT_GROUND_SHARP_SHOCK = 0x7,
    EFFECT_STARS = 0xC,
    EFFECT_SPARKLE = 0xE,
    EFFECT_WATER_RECHARGE = 0xF,
    EFFECT_GROUND_SHOCK = 0x10,
    EFFECT_GROUND_SMOKE_PLUME = 0x11,
    EFFECT_WARP_IN_BODY_PIECES = 0x24,
    EFFECT_WARP_IN_BELT_PIECES = 0x25,
    EFFECT_WARP_IN_HAT_PIECES = 0x26,
    EFFECT_WARP_IN_RED_PIECES = 0x27,
    EFFECT_WARP_IN_BLUE_PIECES = 0x29,
    EFFECT_WARP_IN_BROWN_PIECES = 0x2A,
    EFFECT_WARP_IN_FLUDD_PIECES = 0x2D,
    EFFECT_WATER_RIPPLE = 0x30
  };

  TMario();
  virtual ~TMario();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *receiver, u32 msg) override;
  virtual Mtx44 *getTakingMtx() override;
  virtual u32 moveRequest(const TVec3f &destPosition) override;
  virtual void initValues();
  virtual void checkReturn();
  virtual void checkController();
  virtual void playerControl(JDrama::TGraphics *);
  virtual void initModel();
  virtual void drawSpecial(JDrama::TGraphics *);
  virtual void checkCollision();
  virtual void damageExec(THitActor *, int, int, int, f32, int, f32, s16);
  virtual u8 getVoiceStatus();
  virtual void drawSyncCallback(u16) override;

  // add funcs

  u32 mActionState;   // 0x0074
  u32 mJumpingState;  // 0x0078
  u32 mState;         // 0x007C
  u32 mPrevState;     // 0x0080
  u16 mSubState;      // 0x0084
  u16 mSubStateTimer; // 0x0086
  u32 mJumpSlipState;
  f32 mBaseAcceleration;        // 0x008C
  u16 mAccelerationDirection;   // 0x0090
  u16 _92;                      // padding?
  TVec3s mAngle; // 0x0094
  JGeometry::TVec3<u16> _9A;
  u16 _A0;
  TVec3f mSpeed;     // 0x00A4
  f32 mForwardSpeed;                // 0x00B0
  TVec3f mPrevSpeed; //?
  f32 _C0;
  u16 _C4;
  f32 _C8;
  f32 _CC;
  f32 _D0;
  u8 _D4;
  u8 _D5;
  TBGCheckData *mWallTriangle;       // 0x00D8
  TBGCheckData *mRoofTriangle;       // 0x00DC
  TBGCheckData *mFloorTriangle;      // 0x00E0
  TBGCheckData *mFloorTriangleWater; // 0x00E4
  f32 mCeilingAbove;                 // 0x00E8
  f32 mFloorBelow;                   // 0x00EC
  f32 mWaterHeight;                  // 0x00F0
  u16 _F4;
  u16 mLightID;
  u16 _F8;
  u16 _FA;
  u16 _FC;
  u16 _FE;
  u16 _100;
  f32 _104;
  TMarioControllerWork *mControllerWork; // 0x0108
  u32 _10C;                              // unknown
  u32 _110;                              // unknown
  u32 _114;                              // unknown

  struct {
    u32 _04 : 10;
    bool mIsPerforming : 1;
    bool mIsShineShirt : 1;
    u32 _03 : 2;
    bool mIsWater : 1;
    bool mIsShallowWater : 1;
    bool mHasFludd : 1;
    u32 _02 : 1;
    bool mGainHelmet : 1;
    bool mGainHelmetFlwCamera : 1;
    bool mIsGroundPoundSitUp : 1;
    bool mIsGameOver : 1;
    u32 _01 : 5;
    bool mLeftRecentWater : 1;
    bool mTalkingNPC : 1;
    bool mIsVisible : 1;
    bool mAboveSewerFloor : 1;
    u32 _00 : 1;
  } mAttributes; // 0x0118

  struct {
    u32 _04 : 10;
    bool mIsPerforming : 1;
    bool mIsShineShirt : 1;
    u32 _03 : 2;
    bool mIsWater : 1;
    bool mIsShallowWater : 1;
    bool mHasFludd : 1;
    u32 _02 : 1;
    bool mGainHelmet : 1;
    bool mGainHelmetFlwCamera : 1;
    bool mIsGroundPoundSitUp : 1;
    bool mIsGameOver : 1;
    u32 _01 : 5;
    bool mLeftRecentWater : 1;
    bool mTalkingNPC : 1;
    bool mIsVisible : 1;
    bool mAboveSewerFloor : 1;
    u32 _00 : 1;
  } mPrevAttributes; // 0x011C

  u16 mHealth; // 0x0120
  u16 _122;    // 0x0122
  u16 _124;
  u16 _126;
  u16 _128;
  f32 mWaterHealth;        // 0x012C
  f32 mMaxWaterHealth;     // 0x0130
  u32 _134;                // unknown
  f32 mGraffitiSlopeAngle; // unknown
  u16 mDirtyTimeRun;       // unknown
  u32 _140;                // unknown
  u32 _144;
  u32 _148;
  s16 mInvincibilityFrames;
  u16 _14E;
  u32 _150; // unknown
  u32 _154;
  u32 _158;
  f32 mCollisionXZSize; // 0x015C
  f32 _160;
  f32 _164;
  f32 _168;
  f32 _16C;
  f32 _170;
  f32 _174;
  f32 _178;
  f32 _17C;
  f32 _180;
  f32 _184;
  f32 _188;
  f32 _18C;
  f32 _190;
  f32 _194;
  f32 _198;
  f32 _19C;
  f32 _1A0;
  f32 _1A4;
  f32 _1A8;
  f32 _1AC;
  f32 _1B0;
  f32 _1B4;
  f32 _1B8;
  f32 _1BC;
  Mtx _1C0;
  Mtx _1F0;
  Mtx _220;
  Mtx _250;
  TVec3f mLastPosition; //?       0x0280
  TVec3f mLastRotation; //?       0x028C
  u32 mInitialWater;                   // 0x0298
  TVec3f mLastPos;
  TVec3f mLastGroundedPos; // 0x02A8
  u32 _2B4;
  u16 _2B8;
  u16 mOBTimer; // 0x02BA
  f32 _2BC;
  u32 _2C0;
  Mtx _2C4;
  f32 _2F4;
  f32 _2F8;
  f32 _2FC;
  f32 _300;
  f32 _304;
  f32 _308;
  f32 _30C;
  u32 _310;
  f32 _314;
  Mtx _318;
  f32 _348;
  u16 _34C;
  u16 _34E;
  u32 mGraffitiState;
  f32 _354;
  f32 _358;
  f32 _35C;
  u16 _360;
  u16 _362;
  u16 _364;
  u16 _366;
  f32 _368;
  f32 _36C;
  f32 _370;
  f32 _374;
  f32 _378;
  u16 _37C; // padding?
  u16 _37E;
  u32 _380;
  TTakeActor *mGrabTarget; // 0x0384
  u8 _388;
  u8 mSurfGessoID;       // 0x0389
  f32 mHolderHeightDiff; // 0x038C
  u32 _390;
  J3DDrawBuffer *mDrawBufferA; // 0x0394
  J3DDrawBuffer *mDrawBufferB; // 0x0398
  u32 _39C;
  u32 _3A0;
  u32 _3A4;                  // unknown
  M3UModelMario *mModelData; // 0x03A8
  u32 _3AC;                  // unknown
  J3DModel *mHandModel2R;
  J3DModel *mHandModel2L;
  J3DModel *mHandModel3R;
  J3DModel *mHandModel3L;
  J3DModel *mHandModel4R; // unknown
  u8 _3C4;
  u8 mBindBoneIDArray[12];
  u8 _3D1; // padding?
  u8 _3D2; // padding?
  u8 _3D3; // padding?
  u16 _3D4;
  u16 _3D6;
  f32 _3D8;
  f32 _3DC;
  TMarioCap *mCap;   // 0x03E0
  TWaterGun *mFludd; // 0x03E4
  u32 _3E8;
  f32 _3EC;
  TYoshi *mYoshi;     // 0x03F0
  void *mSurfGesso;   // 0x03F4
  MActor *mTorocco;   // 0x03F8
  MActor *mPinnaRail; // 0x03FC
  MActor *mKoopaRail; // 0x0400
  f32 _404;
  f32 _408;
  f32 _40C;
  u16 _410;
  u16 _412;
  f32 _414;
  f32 _418;
  f32 _41C;
  u32 _420;
  u32 _424;
  f32 _428;
  f32 _42C;
  f32 _430;
  f32 _434;
  f32 _438;
  f32 _43C;
  f32 _440;
  f32 _444;
  f32 _448;
  f32 _44C;
  f32 _450;
  f32 _454;
  f32 _458;
  u32 _45C;
  u32 _460;
  u32 _464;
  u32 _468;
  u32 _46C;
  void *mGoopTexture; // bti
  THitActor _474;
  MAnmSound *mAnmSound; // 0x04DC
  u32 _4E0;
  u32 _4E4;
  u32 _4E8;
  u8 _4EC;
  u8 _4ED;
  f32 _4F0;
  f32 _4F4;
  f32 _4F8;
  TMarioGamePad *mController;     // 0x04FC
  TMarioSoundValues mSoundValues; // 0x0500
  u32 _530;
  u8 _534;
  u16 _536;
  u16 _538;
  u8 _53A;
  u8 _53B;
  u32 _53C;
  f32 _540;
  f32 _544;
  u32 _548;
  u8 _54C;
  u8 _54D;
  u16 _54E;
  u16 _550;
  u16 _552;
  u16 _554;
  u32 _558;
  f32 _55C;
  f32 _560;
  f32 _564;
  f32 _568;
  f32 _56C;
  f32 _570;
  TDeParams mDeParams;                            // 0x0574
  TBodyAngleParams mBodyAngleFreeParams;          // 0x09A0
  TBodyAngleParams mBodyAngleWaterGunParams;      // 0x0A20
  TAttackParams mPunchFenceParams;                // 0x0AA0
  TAttackParams mKickRoofParams;                  // 0x0AD0
  TJumpParams mJumpParams;                        // 0x0B00
  TRunParams mRunParams;                          // 0x0EA0
  TSwimParams mSwimParams;                        // 0x104C
  THangFenceParams mHangFenceParams;              // 0x1298
  THangRoofParams mHangRoofParams;                // 0x1318
  TWireParams mWireParams;                        // 0x1334
  TPullParams mPullBGBeakParams;                  // 0x147C
  TPullParams mPullBGTentacleParams;              // 0x14D4
  TPullParams mPullBGFireWanWanBossTailParams;    // 0x152C
  TPullParams mPullFireWanWanTailParams;          // 0x1584
  TClimbParams mClimbParams;                      // 0x15DC
  TSurfingParams mSurfingWaterRedParams;          // 0x1648
  TSurfingParams mSurfingGroundRedParams;         // 0x181C
  TSurfingParams mSurfingWaterYellowParams;       // 0x19F0
  TSurfingParams mSurfingGroundYellowParams;      // 0x1BC4
  TSurfingParams mSurfingWaterGreenParams;        // 0x1D98
  TSurfingParams mSurfingGroundGreenParams;       // 0x1F6C
  THoverParams mHoverParams;                      // 0x2140
  TDivingParams mDivingParams;                    // 0x2184
  TYoshiParams mYoshiParams;                      // 0x21F0
  TWaterEffectParams mWaterEffectParams;          // 0x22AC
  TControllerParams mControllerParams;            // 0x2340
  TGraffitoParams mGraffitoParams;                // 0x2410
  TDirtyParams mDirtyParams;                      // 0x25BC
  TMotorParams mMotorParams;                      // 0x27A4
  TParticleParams mParticleParams;                // 0x27FC
  TEffectParams mEffectParams;                    // 0x28E0
  TSlipParams mSlipNormalParams;                  // 0x294C
  TSlipParams mSlipOilParams;                     // 0x2A30
  TSlipParams mSlipAllParams;                     // 0x2B14
  TSlipParams mSlipAllSliderParams;               // 0x2BF8
  TSlipParams mSlip45Params;                      // 0x2CDC
  TSlipParams mSlipWaterSlopeParams;              // 0x2DC0
  TSlipParams mSlipWaterGroundParams;             // 0x2EA4
  TSlipParams mSlipYoshiParams;                   // 0x2F88
  TUpperBodyParams mUpperBodyParams;              // 0x306C
  TDmgEnemyParams mDmgEnemyCommonParams;          // 0x30D8
  TDmgEnemyParams mDmgHamukuriParams;             // 0x316C
  TDmgEnemyParams mDmgNamekuriParams;             // 0x3200
  TDmgEnemyParams mDmgHinokuriParams;             // 0x3294
  TDmgEnemyParams mDmgFireParams;                 // 0x3328
  TDmgEnemyParams mDmgBGTentacleParams;           // 0x33BC
  TDmgEnemyParams mDmgBossEelParams;              // 0x3450
  TDmgEnemyParams mDmgHanachanBossParams;         // 0x34E4
  TDmgEnemyParams mDmgPoihanaParams;              // 0x3578
  TDmgEnemyParams mDmgKillerParams;               // 0x360C
  TDmgEnemyParams mDmgLampTrapIronParams;         // 0x36A0
  TDmgEnemyParams mDmgLampTrapSpikeParams;        // 0x3734
  TDmgEnemyParams mDmgEnemyMarioParams;           // 0x37C8
  TDmgEnemyParams mDmgCannotBreathParams;         // 0x385C
  TDmgEnemyParams mDmgGraffitoFireParams;         // 0x38F0
  TDmgEnemyParams mDmgGraffitoPoisonParams;       // 0x3984
  TDmgEnemyParams mDmgGraffitoElecParams;         // 0x3A18
  TDmgEnemyParams mDmgGraffitoLavaParams;         // 0x3AAC
  TDmgEnemyParams mDmgGraffitoWaterSurfaceParams; // 0x3B40
  TDmgEnemyParams mDmgMapCode0Params;             // 0x3BD4
  TDmgEnemyParams mDmgMapCode1Params;             // 0x3C68
  TDmgEnemyParams mDmgMapCode2Params;             // 0x3CFC
  TDmgEnemyParams mDmgMapCode3Params;             // 0x3D90
  TDmgEnemyParams mDmgMapCode4Params;             // 0x3E24
  TDmgEnemyParams mDmgMapCode5Params;             // 0x3EB8
  TDmgEnemyParams mDmgMapCode6Params;             // 0x3F4C
  TDmgEnemyParams mDmgMapCode7Params;             // 0x3FE0
  TDmgEnemyParams mDmgMapCode8Params;             // 0x4074
  TDmgEnemyParams mDmgMapCode9Params;             // 0x4108
  TDemoParams mDemoParams;                        // 0x419C
  TSoundParams mSoundParams;                      // 0x4230
  TOptionParams mOptionParams;                    // 0x424C
};