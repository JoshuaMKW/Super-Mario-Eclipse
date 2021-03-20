#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/J3D/J3DDrawBuffer.hxx"

#include "sms/actor/TakeActor.hxx"
#include "sms/actor/Yoshi.hxx"
#include "sms/m3d/M3UModel.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/params/BaseParam.hxx"
#include "sms/params/Params.hxx"

#include "params/MarioParams.hxx"

class TMario;

class TDrawSyncCallback
{
};

struct TMarioAnimeData
{
    enum FLUDD
    {
        FLUDD_ENABLED = 68,
        FLUDD_DISABLED = 200
    };

    enum HAND
    {
        HAND_A,
        HAND_B,
        HAND_C
    };

    bool isPumpOK() const;

    u16 mAnimID;
    u16 mFluddEnabled; //Note: ENUM? 68 = enabled, 200 = disabled
    u8 mAnmTexPattern; //Note: 23 is the max value allowed
    u8 mMarioHand;     //Note: 1, 2, and 3 seem to be valid values, this determines the hand model used(?)
    u8 unk_1;          //Note: If bit 30 is set, it seems to activate a bit flag to multiple pointers, likely mtx related
    u8 unk_2;          //Note: Value seems to be set but is never used?
};

class TMarioCap
{
public:
    TMarioCap(TMario *);

    virtual void perform(u32, JDrama::TGraphics *);

    void createMirrorModel();
    void mtxEffectHide();
    void mtxEffectShow();
};

class M3UModelMario : public M3UModel
{
public:
    virtual void updateIn() override;
    virtual void updateOut() override;

    void changeMtxCalcSIAnmBQAnmTransform(int, int, u8);
    void updateInMotion();
};

class TMarioSoundValues
{
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
    u8 _2A;
    u8 _2B;
    u8 _2C;
};

struct TMarioControllerWork
{
    enum Buttons
    {
        R = 0x20,
        A = 0x100,
        B = 0x200
    };

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

class TMarioGamePad : public JUTGamePad
{
public:
    virtual ~TMarioGamePad();

    void onNeutralMarioKey();
    void read();
    void reset();
    void updateMeaning();

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

    struct
    {
        u16 _00 : 8;
        bool mDisable : 1;
        u8 _01 : 5;
        bool mReadInput : 1;
        u8 _02 : 1;
    } State; //0x00E2

    u16 _E4;
    u16 _E6; // padding?
    u32 _E8;
    u32 _EC; // padding?
};

class TMario : public TTakeActor, public TDrawSyncCallback
{
public:
    struct TDeParams : public TParams
    {
        TDeParams();

        TParamT<s16> mHPMax;           // Max health
        TParamT<f32> mRunningMax;      // Max running speed
        TParamT<f32> mDashMax;         // Turbo Nozzle max speed
        TParamT<f32> mDashAcc;         // Turbo Nozzle acceleration
        TParamT<f32> mDashBrake;       // Turbo Nozzle stop speed?
        TParamT<f32> mDashStartTime;   // Turbo Nozzle start
        TParamT<f32> mWaitingRotSp;    // Waiting rotation speed
        TParamT<f32> mRunningRotSpMin; // Running rotation speed
        TParamT<f32> mRunningRotSpMax; // Running rotation speed max
        TParamT<f32> mRocketRotSp;     // Rocket Nozzle rotation speed
        TParamT<f32> mPumpingRotSpMin; // Pumping rotation speed min
        TParamT<f32> mPumpingRotSpMax; // Pumping rotation speed max
        TParamT<f32> mInvincibleTime;
        TParamT<f32> mFootPrintTimerMax;
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

    struct TBodyAngleParams : public TParams
    {
        TBodyAngleParams(const char *prm)
            : mHeadRot(0.0f, "mHeadRot"),
              mWaistRoll(0.0f, "mWaistRoll"),
              mWaistPitch(80.0f, "mWaistPitch"),
              mWaistRollMax(0, "mWaistRollMax"),
              mWaistPitchMax(1000, "mWaistPitchMax"),
              mWaistAngleChangeRate(0.07f, "mWaistAngleChangeRate")
        {
            load(prm);
        }

        TParamT<f32> mHeadRot;
        TParamT<f32> mWaistRoll;
        TParamT<f32> mWaistPitch;
        TParamT<s16> mWaistRollMax;
        TParamT<s16> mWaistPitchMax;
        TParamT<s16> mWaistAngleChangeRate;
    };

    struct TAttackParams : public TParams
    {
        TAttackParams(const char *prm)
            : mRadius(100.0f, "mRadius"),
              mHeight(50.0f, "mHeight")
        {
            load(prm);
        }

        TParamT<f32> mRadius;
        TParamT<f32> mHeight;
    };

    struct TJumpParams : public TParams
    {
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
        TParamT<f32> mValleyDepth; // Had 2 of these by accident, validate!
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
        TParamT<f32> mGetOffYoshiY;
    };

    struct TRunParams : public TParams
    {
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

    struct TSwimParams : public TParams
    {
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

    struct THangFenceParams : public TParams
    {
        THangFenceParams()
            : mMoveSp(0.1f, "mMoveSp"),
              mAnmRate(0.5f, "mAnmRate"),
              mRapidTime(2000, "mRapidTime"),
              mLimitTime(2400, "mLimitTime"),
              mAnmRapid(8.0f, "mAnmRapid"),
              mDescentSp(10.0f, "mDescentSp")
        {
            load("/Mario/Hanging.prm");
        }

        TParamT<f32> mMoveSp;
        TParamT<f32> mAnmRate;
        TParamT<s16> mRapidTime;
        TParamT<s16> mLimitTime;
        TParamT<f32> mAnmRapid;
        TParamT<f32> mDescentSp;
    };

    struct THangRoofParams : public TParams
    {
        THangRoofParams()
            : mAnmMult(0.3f, "mAnmMult")
        {
            load("/Mario/HangRoof.prm");
        }

        TParamT<f32> mAnmMult;
    };

    struct TWireParams : public TParams
    {
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

    struct TPullParams : public TParams
    {
        TPullParams(const char *prm)
            : mPullRateV(0.3f, "mPullRateV"),
              mPullRateH(0.05f, "mPullRateH"),
              mOilPullRateV(0.1f, "mOilPullRateV"),
              mOilPullRateH(0.01f, "mOilPullRateH")
        {
            load(prm);
        }

        TParamT<f32> mPullRateV;
        TParamT<f32> mPullRateH;
        TParamT<f32> mOilPullRateV;
        TParamT<f32> mOilPullRateH;
    };

    struct TClimbParams : public TParams
    {
        TClimbParams()
            : mClimbSp(0.035f, "mClimbSp"),
              mRotateSp(3.0f, "mRotateSp"),
              mClimbAnmRate(1.0f / 256.0f, "mClimbAnmRate"),
              mCatchRadius(100.0f, "mCatchRadius"),
              mCatchAngle(0.8f, "mCatchAngle")
        {
            load("/Mario/Bar.prm");
        }

        TParamT<f32> mClimbSp;
        TParamT<f32> mRotateSp;
        TParamT<f32> mClimbAnmRate;
        TParamT<f32> mCatchRadius;
        TParamT<f32> mCatchAngle;
    };

    struct TSurfingParams : public TParams
    {
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

    struct THoverParams : public TParams
    {
        THoverParams()
            : mRotSp(128, "mRotSp"),
              mAccelRate(0.03f, "mAccelRate"),
              mBrake(0.95f, "mBrake")
        {
            load("/Mario/HHover.prm");
        }

        TParamT<s16> mRotSp;
        TParamT<f32> mAccelRate;
        TParamT<f32> mBrake;
    };

    struct TDivingParams : public TParams
    {
        TDivingParams()
            : mRotSp(128, "mRotSp"),
              mGravity(0.5f, "mGravity"),
              mAccelControl(0.02f, "mAccelControl"),
              mSeaBrake(0.999f, "mSeaBrake"),
              mSeaBrakeY(0.98f, "mSeaBrakeY")
        {
            load("/Mario/Diving.prm");
        }

        TParamT<s16> mRotSp;
        TParamT<f32> mGravity;
        TParamT<f32> mAccelControl;
        TParamT<f32> mSeaBrake;
        TParamT<f32> mSeaBrakeY;
    };

    struct TYoshiParams : public TParams
    {
        TYoshiParams()
            : mRunYoshiMult(1.2f, "mRunYoshiMult"),
              mJumpYoshiMult(1.0f, "mJumpYoshiMult"),
              mRotYoshiMult(1.5f, "mRotYoshiMult"),
              mHeadFront(80.0f, "mHeadFront"),
              mHeadRadius(50.0f, "mHeadRadius"),
              mHoldOutAccCtrlF(0.01f, "mHoldOutAccCtrlF"),
              mHoldOutAccCtrlB(0.023f, "mHoldOutAccCtrlB"),
              mHoldOutSldCtrl(0.3f, "mHoldOutSldCtrl"),
              mDecBrake(1.0f, "mDecBrake")
        {
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

    struct TWaterEffectParams : public TParams
    {
        TWaterEffectParams()
            : mJumpIntoMdlEffectSpY(10.0f, "mJumpIntoMdlEffectSpY"),
              mJumpIntoMinY(20.0f, "mJumpIntoMinY"),
              mJumpIntoMaxY(50.0f, "mJumpIntoMaxY"),
              mJumpIntoScaleMin(0.75f, "mJumpIntoScaleMin"),
              mJumpIntoScaleWidth(1.0f, "mJumpIntoScaleWidth"),
              mRunningRippleSpeed(30.0f, "mRunningRippleSpeed"),
              mRunningRippleDepth(30.0f, "mRunningRippleDepth")
        {
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

    struct TControllerParams : public TParams
    {
        TControllerParams()
            : mAnalogLRToZeroVal(30, "mAnalogLRToZeroVal"),
              mAnalogLRToMiddleVal(90, "mAnalogLRToMiddleVal"),
              mAnalogLRToMaxVal(150, "mAnalogLRToMaxVal"),
              mAnalogLRMiddleVal(0.1f, "mAnalogLRMiddleVal"),
              mStartToWalkLevel(15.0f, "mStartToWalkLevel"),
              mStickRotateTime(24, "mStickRotateTime"),
              mLengthMultTimes(10, "mLengthMultTimes"),
              mLengthMult(0.935f, "mLengthMult"),
              mSquatRotMidAnalog(0.7f, "mSquatRotMidAnalog"),
              mSquatRotMidValue(0.05f, "mSquatRotMidValue")
        {
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

    struct TGraffitoParams : public TParams
    {
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

    struct TDirtyParams : public TParams
    {
        TDirtyParams();

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

    enum State {
        NUMBER = 0x0000000F,
        DOJUMP = 0x00000080,
        AIRBORN = 0x00000800,
        CUTSCENE = 0x00001000,
        WATERBORN = 0x00002000,
        RUNNING = 0x04000440,
        IDLE = 0x0C400201,
        STOP = 0x0C00023D,
        SPIN = 0x00000441,
        JUMPSPIN = 0x00000890,
        JUMPSPIN1 = 0x00000895,
        JUMPSPIN2 = 0x00000896,
        JUMP = 0x02000880,
        D_JUMP = 0x02000881,
        TRIPLE_J = 0x00000882,
        JMP_LAND = 0x04000470,
        HVY_LAND = 0x04000473,
        D_LAND = 0x04000472,
        T_LAND = 0x0800023A,
        JUMPSIDE = 0x00000880,
        BOUNCE = 0x00000884,
        SIDESTEP = 0x0C008220,
        SIDE_FLIP = 0x00000887,
        FALL = 0x0000088C,
        SWIM = 0x000024D7,
        DIVE = 0x0080088A,
        DIVEJUMP = 0x02000889,
        DIVESLIDE = 0x00800456,
        GOOPSLIDE = 0x0084045D,
        CLIMB = 0x18100340,
        CLIMBUP = 0x10100343,
        WALLJUMP = 0x02000886,
        WALLSLIDE = 0x000008A7,
        ROOFHANG = 0x00200349,
        HANG = 0x3800034B,
        HANGCLIMB = 0x3000054F,
        SLAMSTART = 0x008008A9,
        SLAM = 0x0080023C,
        SPRAY = 0x0C008220,
        THROWN = 0x000208B8,
        HOVER = 0x0000088B,
        STUCK = 0x0002033C,
        TALKING = 0x10001308,
        TURNING = 0x00000444,
        YOSHI_ESC = 0x0000089C,
        SHINE_C = 0x00001302, // Collected Shine Sprite
        DEATH = 0x00020467,
        DOOR_F_O = 0x00001321, // Door open fail
        WALL_S_L = 0x04000471,
        F_KNCK_H = 0x000208B0, // hard knockback from front (bumping into a wall from dive)
        KNCK_LND = 0x00020462, // Landing from front knockback
        KNCK_GND = 0x00020466, // Front knockback while grounded
        FIRE_HIT = 0x000208B7,
        FIRE_RVR = 0x08000239, // Recover from fire on ground
        HOVER_F = 0x0000088D,  // Falling from hover
        SLIDE_R1 = 0x000008A6, // Recover from slide by flipping
        SLIDE_R2 = 0x00000386, // Recover from slide by getting up
        R_SPRAY = 0x0C008220,  // Recover from spray
        G_POUND = 0x008008A9,  // Ground pounding
        NPC_PUTDOWN = 0x80000387,
        NPC_THROW = 0x80000588,
        NPC_JUMPTHROW = 0x820008AB
    };

    enum Status
    {
        ALIVE = 0x1,
        SEWER_FLOOR = 0x2,
        INVISIBLE = 0x4,
        NPC_TALKING = 0x8,
        LEAVING_WATER = 0x10,
        SLIP = 0x80,
        GAMEOVER = 0x400,
        UP_GROUND_POUND = 0x800,
        HASHELMET_FOLLOWCAMERA = 0x1000,
        HASHELMET = 0x2000,
        HASFLUDD = 0x8000,
        SPLASH = 0x10000,
        PUDDLE = 0x20000,
        SHIRT = 0x100000,
        GONE = 0x200000
    };

    enum Voice
    {
        CANNON_WAIL = 30911,
        TRIPLE_JUMP = 30902,
        JUMP = 30891,
        DIVE_OUT = 30897,
        FALL_LEDGE_GRAB = 30944
    };

    enum Animation
    {
        WALLHANG = 0x33,
        FALL = 0x4C,
        BOUNCE = 0x50,
        IDLE = 0xC3,
        SHINEGET = 0xCD,
        SPINJUMP = 0xF4
    };

    enum Effect
    {
        SMOKE_CLOUD = 0x1,
        ROCKET_SPRAY_MIST = 0x2,
        ROCKET_SPRAY_DROPS = 0x3,
        BURNING_SMOKE = 0x6,
        GROUND_SHARP_SHOCK = 0x7,
        STARS = 0xC,
        SPARKLE = 0xE,
        WATER_RECHARGE = 0xF,
        GROUND_SHOCK = 0x10,
        GROUND_SMOKE_PLUME = 0x11,
        WARP_IN_BODY_PIECES = 0x24,
        WARP_IN_BELT_PIECES = 0x25,
        WARP_IN_HAT_PIECES = 0x26,
        WARP_IN_RED_PIECES = 0x27,
        WARP_IN_BLUE_PIECES = 0x29,
        WARP_IN_BROWN_PIECES = 0x2A,
        WARP_IN_FLUDD_PIECES = 0x2D,
        WATER_RIPPLE = 0x30
    };

    u32 mActionState;                      //0x0074
    u32 mJumpingState;                     //0x0078
    u32 mState;                            //0x007C
    u32 mPrevState;                        //0x0080
    u16 mSubState;                         //0x0084
    u16 mSubStateTimer;                    //0x0086
    u32 _88;
    f32 mBaseAcceleration;                 //0x008C
    u16 mAccelerationDirection;            //0x0090
    u16 _92; // padding?
    JGeometry::TVec3<u16> mAngle;          //0x0094
    JGeometry::TVec3<u16> _9A;
    u16 _A0;
    JGeometry::TVec3<f32> mSpeed;          //0x00A4
    f32 mForwardSpeed;                     //0x00B0
    f32 _B4;
    f32 _B8;
    f32 _BC;
    f32 _C0;
    u16 _C4;
    f32 _C8;
    f32 _CC;
    f32 _D0;
    u8 _D4;
    u8 _D5;
    TBGCheckData *mWallTriangle;           //0x00D8
    TBGCheckData *mRoofTriangle;           //0x00DC
    TBGCheckData *mFloorTriangle;          //0x00E0
    TBGCheckData *mFloorTriangleCopy;      //0x00E4
    f32 mCeilingAbove;                     //0x00E8
    f32 mFloorBelow;                       //0x00EC
    f32 mWaterHeight;                      //0x00F0
    u16 _F4;
    u16 _F6;
    u16 _F8;
    u16 _FA;
    u16 _FC;
    u16 _FE;
    u16 _100;
    f32 _104;
    TMarioControllerWork *mControllerWork; //0x0108
    u32 _10C; //unknown
    u32 _110; //unknown
    u32 _114; //unknown

    struct
    {
        u32 _04 : 10;
        bool mIsGone : 1;
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
    } mAttributes; //0x0118

    struct
    {
        u32 _04 : 10;
        bool mIsGone : 1;
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
    } mPrevAttributes; //0x011C

    u16 mHealth;                                 //0x0120
    u16 _122;                                     //0x0122
    u16 _124;
    u16 _126;
    u16 _128;
    f32 mWaterHealth;                            //0x012C
    f32 mMaxWaterHealth;                         //0x0130
    u32 _134; // unknown
    u32 _138; // unknown
    u32 _13C; // unknown
    u32 _140; // unknown
    u32 _144;
    u32 _148;
    u16 _14C;
    u16 _14E;
    u32 _150; // unknown
    u32 _154;
    u32 _158;
    f32 mCollisionXZSize;                        //0x015C
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
    JGeometry::TVec3<f32> mLastPosition; //?       0x0280
    JGeometry::TVec3<f32> mLastRotation; //?       0x028C
    u32 mInitialWater;                           //0x0298
    JGeometry::TVec3<f32> _29C;
    JGeometry::TVec3<f32> mLastGroundedPos;      //0x02A8
    u32 _2B4;
    u16 _2B8;
    u16 mOBTimer;                                //0x02BA
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
    u32 _350;
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
    TTakeActor *mGrabTarget;                     //0x0384
    u8 _388;
    u8 mSurfGessoID;                             //0x0389
    f32 mHolderHeightDiff;                       //0x038C
    u32 _390;
    J3DDrawBuffer *mDrawBufferA;                 //0x0394
    J3DDrawBuffer *mDrawBufferB;                 //0x0398
    u32 _39C;
    u32 _3A0;
    u32 _3A4; // unknown
    M3UModelMario *mModelData;                   //0x03A8
    u32 _3AC; // unknown
    u32 _3B0;
    u32 _3B4;
    u32 _3B8;
    u32 _3BC;
    u32 _3C0; // unknown
    u8 _3C4; // padding?
    u8 mBindBoneIDArray[12];                     //0x03C5
    u8 _3D1; // padding?
    u8 _3D2; // padding?
    u8 _3D3; // padding?
    u16 _3D4;
    u16 _3D6;
    f32 _3D8;
    f32 _3DC;
    TMarioCap *mCap;                             //0x03E0
    TWaterGun *mFludd;                           //0x03E4
    u32 _3E8;
    f32 _3EC;
    TYoshi *mYoshi;                              //0x03F0
    void *mSurfGesso;                            //0x03F4
    void *_3F8;
    void *_3FC;
    MarioParams *mCustomInfo;                    //0x0400
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
    THitActor _474;                              //0x0474
    u32 _4DC;
    u32 _4E0;
    u32 _4E4;
    u32 _4E8;
    u8 _4EC;
    u8 _4ED;
    f32 _4F0;
    f32 _4F4;
    f32 _4F8;
    TMarioGamePad *mController;                  //0x04FC
    TMarioSoundValues mSoundValues;              //0x0500
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
    TDeParams mDeParams;                         //0x0574
    TBodyAngleParams mBodyAngleFreeParams;       //0x09A0
    TBodyAngleParams mBodyAngleWaterGunParams;   //0x0A20
    TAttackParams mPunchFenceParams;             //0x0AA0
    TAttackParams mKickRoofParams;               //0x0AD0
    TJumpParams mJumpParams;                     //0x0B00
    TRunParams mRunParams;                       //0x0EA0
    TSwimParams mSwimParams;                     //0x104C
    THangFenceParams mHangFenceParams;           //0x1298
    THangRoofParams mHangRoofParams;             //0x1318
    TWireParams mWireParams;                     //0x1334
    TPullParams mPullBGBeakParams;               //0x147C
    TPullParams mPullBGTentacleParams;           //0x14D4
    TPullParams mPullBGFireWanWanBossTailParams; //0x152C
    TPullParams mPullFireWanWanTailParams;       //0x1584
    TSurfingParams mSurfingWaterRedParams;       //0x1648
    TSurfingParams mSurfingGroundRedParams;      //0x181C
    TSurfingParams mSurfingWaterYellowParams;    //0x19F0
    TSurfingParams mSurfingGroundYellowParams;   //0x1BC4
    TSurfingParams mSurfingWaterGreenParams;     //0x1D98
    TSurfingParams mSurfingGroundGreenParams;    //0x1F6C
    THoverParams mHoverParams;                   //0x2140
    TDivingParams mDivingParams;                 //0x2184
    TYoshiParams mYoshiParams;                   //0x21F0
    TWaterEffectParams mWaterEffectParams;       //0x22AC
    TControllerParams mControllerParams;         //0x2340
    TGraffitoParams mGraffitoParams;             //0x2410
    TDirtyParams mDirtyParams;                   //0x25BC
    //0x27A4

    //todo: dirtyparams, graffitoparams, and misc

    //end = 0x4290
};