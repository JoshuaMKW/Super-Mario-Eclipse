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

        TParamRT<s16> mHPMax;           // Max health
        TParamRT<f32> mRunningMax;      // Max running speed
        TParamRT<f32> mDashMax;         // Turbo Nozzle max speed
        TParamRT<f32> mDashAcc;         // Turbo Nozzle acceleration
        TParamRT<f32> mDashBrake;       // Turbo Nozzle stop speed?
        TParamRT<f32> mDashStartTime;   // Turbo Nozzle start
        TParamRT<f32> mWaitingRotSp;    // Waiting rotation speed
        TParamRT<f32> mRunningRotSpMin; // Running rotation speed
        TParamRT<f32> mRunningRotSpMax; // Running rotation speed max
        TParamRT<f32> mRocketRotSp;     // Rocket Nozzle rotation speed
        TParamRT<f32> mPumpingRotSpMin; // Pumping rotation speed min
        TParamRT<f32> mPumpingRotSpMax; // Pumping rotation speed max
        TParamRT<f32> mInvincibleTime;
        TParamRT<f32> mFootPrintTimerMax;
        TParamRT<f32> mWaterTriggerRate;
        TParamRT<f32> mGraffitoNoDmgTime;
        TParamRT<f32> mRestMax;
        TParamRT<f32> mShadowSize;
        TParamRT<f32> mShadowErase;
        TParamRT<f32> mHoldRadius;
        TParamRT<f32> mDamageRadius;
        TParamRT<f32> mDamageHeight;
        TParamRT<f32> mAttackHeight;
        TParamRT<f32> mTrampleRadius;
        TParamRT<f32> mPushupRadius;
        TParamRT<f32> mPushupHeight;
        TParamRT<f32> mHipdropRadius;
        TParamRT<f32> mQuakeRadius;
        TParamRT<f32> mQuakeHeight;
        TParamRT<f32> mTramplePowStep1;
        TParamRT<f32> mTramplePowStep2;
        TParamRT<f32> mTramplePowStep3;
        TParamRT<f32> mJumpWallHeight;
        TParamRT<f32> mThrowPower;
        TParamRT<f32> mSlipStart;
        TParamRT<f32> mWasOnWaterSlip;
        TParamRT<f32> mInWaterSlip;
        TParamRT<f32> mToroccoRotSp;
        TParamRT<s16> mRecoverTimer;
        TParamRT<s16> mHotTimer;
        TParamRT<f32> mFeelDeep;
        TParamRT<f32> mDamageFallHeight;
        TParamRT<f32> mForceSlipAngle;
        TParamRT<f32> mClashSpeed; // Wall bonk speed?
        TParamRT<f32> mHangWallMovableAngle;
        TParamRT<f32> mColMvMax;
        TParamRT<s16> mNoFreezeTime;   // Wall hang no freeze timer
        TParamRT<s16> mKickFreezeTime; // Wall hang freeze timer
        TParamRT<s16> mSurfStartFreezeTime;
        TParamRT<f32> mSleepingCheckDist;
        TParamRT<f32> mSleepingCheckHeight;
        TParamRT<s16> mIllegalPlaneCtInc; // Death plane timer increment
        TParamRT<s16> mIllegalPlaneTime;  // Death plane timer max
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

        TParamRT<f32> mHeadRot;
        TParamRT<f32> mWaistRoll;
        TParamRT<f32> mWaistPitch;
        TParamRT<s16> mWaistRollMax;
        TParamRT<s16> mWaistPitchMax;
        TParamRT<s16> mWaistAngleChangeRate;
    };

    struct TAttackParams : public TParams
    {
        TAttackParams(const char *prm)
            : mRadius(100.0f, "mRadius"),
              mHeight(50.0f, "mHeight")
        {
            load(prm);
        }

        TParamRT<f32> mRadius;
        TParamRT<f32> mHeight;
    };

    struct TJumpParams : public TParams
    {
        TJumpParams();

        TParamRT<f32> mGravity;
        TParamRT<f32> mSpinJumpGravity;
        TParamRT<f32> mJumpingMax;
        TParamRT<f32> mJumpSpeedBrake;
        TParamRT<f32> mJumpSpeedAccelControl;
        TParamRT<f32> mJumpSlideControl;
        TParamRT<f32> mTurnJumpForce;
        TParamRT<f32> mFenceSpeed;
        TParamRT<f32> mFireDownForce;
        TParamRT<f32> mFireDownControl;
        TParamRT<f32> mFireBackVelocity;
        TParamRT<f32> mBroadJumpForce;
        TParamRT<f32> mBroadJumpForceY;
        TParamRT<f32> mRotateJumpForceY;
        TParamRT<f32> mPopUpSpeedY;
        TParamRT<f32> mPopUpForceYMult;
        TParamRT<f32> mBackJumpForce;
        TParamRT<f32> mBackJumpForceY;
        TParamRT<f32> mHipAttackSpeedY;
        TParamRT<f32> mSuperHipAttackSpeedY;
        TParamRT<f32> mJumpCatchRotXSp;
        TParamRT<f32> mJumpCatchRotXMax;
        TParamRT<f32> mRotBroadEnableV;
        TParamRT<f32> mRotBroadJumpForce;
        TParamRT<f32> mRotBroadJumpForceY;
        TParamRT<f32> mTrampolineDec;
        TParamRT<f32> mSecJumpEnableSp;
        TParamRT<f32> mSecJumpForce;
        TParamRT<f32> mSecJumpSpeedMult;
        TParamRT<f32> mSecJumpXZMult;
        TParamRT<f32> mTriJumpEnableSp;
        TParamRT<f32> mUltraJumpForce;
        TParamRT<f32> mUltraJumpSpeedMult;
        TParamRT<f32> mUltraJumpXZMult;
        TParamRT<f32> mValleyDepth; // Had 2 of these by accident, validate!
        TParamRT<f32> mThrownAccel;
        TParamRT<f32> mThrownSlide;
        TParamRT<f32> mThrownBrake;
        TParamRT<f32> mTremblePower;
        TParamRT<f32> mTrembleAccele;
        TParamRT<f32> mTrembleBrake;
        TParamRT<s16> mTrembleTime;
        TParamRT<s16> mClashAngle;
        TParamRT<f32> mJumpJumpCatchSp;
        TParamRT<f32> mGetOffYoshiY;
        TParamRT<s16> mSuperHipAttackCt; // Time until super hip attack?
        TParamRT<f32> mGetOffYoshiY;
    };

    struct TRunParams : public TParams
    {
        TRunParams();

        TParamRT<f32> mMaxSpeed;
        TParamRT<f32> mVelMinusBrake;
        TParamRT<f32> mAddBase;
        TParamRT<f32> mAddVelDiv;
        TParamRT<f32> mDecStartNrmY;
        TParamRT<f32> mDecBrake;
        TParamRT<f32> mSoft2Walk;
        TParamRT<f32> mWalk2Soft;
        TParamRT<f32> mSoftStepAnmMult;
        TParamRT<f32> mRunAnmSpeedBase;
        TParamRT<f32> mRunAnmSpeedMult;
        TParamRT<f32> mMotBlendWalkSp;
        TParamRT<f32> mMotBlendRunSp;
        TParamRT<f32> mSwimDepth;
        TParamRT<f32> mInWaterBrake;
        TParamRT<f32> mInWaterAnmBrake;
        TParamRT<f32> mPumpingSlideSp;
        TParamRT<f32> mPumpingSlideAnmSp;
        TParamRT<f32> mDoJumpCatchSp;
        TParamRT<f32> mTurnNeedSp;
        TParamRT<s16> mDashRotSp;
    };

    struct TSwimParams : public TParams
    {
        TSwimParams();

        TParamRT<f32> mStartSp;
        TParamRT<f32> mMoveSp;
        TParamRT<f32> mMoveBrake;
        TParamRT<s16> mSwimmingRotSpMin;
        TParamRT<s16> mSwimmingRotSpMax;
        TParamRT<s16> mPumpingRotSpMin;
        TParamRT<s16> mPumpingRotSpMax;
        TParamRT<f32> mGravity;
        TParamRT<f32> mWaitBouyancy;
        TParamRT<f32> mMoveBouyancy;
        TParamRT<f32> mUpDownBrake;
        TParamRT<f32> mCanJumpDepth;
        TParamRT<f32> mEndDepth;
        TParamRT<f32> mFloatHeight;
        TParamRT<f32> mStartVMult;
        TParamRT<f32> mStartVYMult;
        TParamRT<f32> mRush;
        TParamRT<f32> mAnmBrake;
        TParamRT<f32> mPaddleSpeedUp;
        TParamRT<f32> mPaddleJumpUp;
        TParamRT<f32> mFloatUp;
        TParamRT<f32> mWaterLevelCheckHeight;
        TParamRT<f32> mPaddleDown;
        TParamRT<s16> mWaitSinkTime;
        TParamRT<f32> mCanBreathDepth;
        TParamRT<f32> mWaitSinkSpeed;
        TParamRT<f32> mAirDec;
        TParamRT<f32> mAirDecDive;
        TParamRT<f32> mAirInc;
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

        TParamRT<f32> mMoveSp;
        TParamRT<f32> mAnmRate;
        TParamRT<s16> mRapidTime;
        TParamRT<s16> mLimitTime;
        TParamRT<f32> mAnmRapid;
        TParamRT<f32> mDescentSp;
    };

    struct THangRoofParams : public TParams
    {
        THangRoofParams()
            : mAnmMult(0.3f, "mAnmMult")
        {
            load("/Mario/HangRoof.prm");
        }

        TParamRT<f32> mAnmMult;
    };

    struct TWireParams : public TParams
    {
        TWireParams()
            : mRotSpeed(-8, "mRotSpeed"),
              mRotSpeedTrgHover(8, "mRotSpeedTrgHover"),
              mRotSpeedTrgTurbo(1000, "mRotSpeedTrgTurbo"),
              mRotSpeedTrgRocket(1000, "mRotSpeedTrgRocket"),
              mRotSpeedMax(1400, "mRotSpeedMax"),
              mRotStop(100, "mRotStop"),
              mRotGravity(20, "mRotGravity"),
              mRotBrake(0.98f, "mRotBrake"),
              mJumpRate(0.09f, "mJumpRate"),
              mSwingRate(0.005f, "mSwingRate"),
              mWireJumpAccelControl(0.01f, "mWireJumpAccelControl"),
              mWireJumpSlideControl(0.3f, "mWireJumpSlideControl"),
              mWireJumpMult(5.0f, "mWireJumpMult"),
              mWireJumpBase(20.0f, "mWireJumpBase"),
              mWireSwingBrake(0.99f, "mWireSwingBrake"),
              mWireSwingMax(100.0f, "mWireSwingMax")
        {
            load("/Mario/HangRoof.prm");
        }

        TParamRT<s16> mRotSpeed;
        TParamRT<s16> mRotSpeedTrgHover;
        TParamRT<s16> mRotSpeedTrgTurbo;
        TParamRT<s16> mRotSpeedTrgRocket;
        TParamRT<s16> mRotSpeedMax;
        TParamRT<s16> mRotStop;
        TParamRT<s16> mRotGravity;
        TParamRT<f32> mRotBrake;
        TParamRT<f32> mJumpRate;
        TParamRT<f32> mSwingRate;
        TParamRT<f32> mWireJumpAccelControl;
        TParamRT<f32> mWireJumpSlideControl;
        TParamRT<f32> mWireJumpMult;
        TParamRT<f32> mWireJumpBase;
        TParamRT<f32> mWireSwingBrake;
        TParamRT<f32> mWireSwingMax;
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

        TParamRT<f32> mPullRateV;
        TParamRT<f32> mPullRateH;
        TParamRT<f32> mOilPullRateV;
        TParamRT<f32> mOilPullRateH;
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

        TParamRT<f32> mClimbSp;
        TParamRT<f32> mRotateSp;
        TParamRT<f32> mClimbAnmRate;
        TParamRT<f32> mCatchRadius;
        TParamRT<f32> mCatchAngle;
    };

    struct TSurfingParams : public TParams
    {
        TSurfingParams(const char *prm);

        TParamRT<f32> mRotMin;
        TParamRT<f32> mRotMax;
        TParamRT<f32> mPowMin;
        TParamRT<f32> mPowMax;
        TParamRT<f32> mAccel;
        TParamRT<f32> mWaistRoll;
        TParamRT<f32> mWaistPitch;
        TParamRT<s16> mWaistRollMax;
        TParamRT<s16> mWaistPitchMax;
        TParamRT<f32> mRoll;
        TParamRT<f32> mPitch;
        TParamRT<s16> mRollMax;
        TParamRT<s16> mPitchMax;
        TParamRT<f32> mAngleChangeRate;
        TParamRT<f32> mWaistAngleChangeRate;
        TParamRT<f32> mScaleMin;
        TParamRT<f32> mScaleMax;
        TParamRT<f32> mScaleMinSpeed;
        TParamRT<f32> mScaleMaxSpeed;
        TParamRT<f32> mJumpPow;
        TParamRT<f32> mJumpXZRatio;
        TParamRT<f32> mClashSpeed;
        TParamRT<s16> mClashAngle;
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

        TParamRT<s16> mRotSp;
        TParamRT<f32> mAccelRate;
        TParamRT<f32> mBrake;
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

        TParamRT<s16> mRotSp;
        TParamRT<f32> mGravity;
        TParamRT<f32> mAccelControl;
        TParamRT<f32> mSeaBrake;
        TParamRT<f32> mSeaBrakeY;
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

        TParamRT<f32> mRunYoshiMult;
        TParamRT<f32> mJumpYoshiMult;
        TParamRT<f32> mRotYoshiMult;
        TParamRT<f32> mHeadFront;
        TParamRT<f32> mHeadRadius;
        TParamRT<f32> mHoldOutAccCtrlF;
        TParamRT<f32> mHoldOutAccCtrlB;
        TParamRT<f32> mHoldOutSldCtrl;
        TParamRT<f32> mDecBrake;
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

        TParamRT<f32> mJumpIntoMdlEffectSpY;
        TParamRT<f32> mJumpIntoMinY;
        TParamRT<f32> mJumpIntoMaxY;
        TParamRT<f32> mJumpIntoScaleMin;
        TParamRT<f32> mJumpIntoScaleWidth;
        TParamRT<f32> mRunningRippleSpeed;
        TParamRT<f32> mRunningRippleDepth;
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

        TParamRT<u8> mAnalogLRToZeroVal;
        TParamRT<u8> mAnalogLRToMiddleVal;
        TParamRT<u8> mAnalogLRToMaxVal;
        TParamRT<f32> mAnalogLRMiddleVal;
        TParamRT<f32> mStartToWalkLevel;
        TParamRT<s16> mStickRotateTime;
        TParamRT<s16> mLengthMultTimes;
        TParamRT<f32> mLengthMult;
        TParamRT<f32> mSquatRotMidAnalog;
        TParamRT<f32> mSquatRotMidValue;
    };

    struct TGraffitoParams : public TParams
    {
        TGraffitoParams();

        TParamRT<s16> mSinkTime;
        TParamRT<s16> mSinkDmgTime;
        TParamRT<f32> mSinkHeight;
        TParamRT<f32> mSinkMoveMin;
        TParamRT<f32> mSinkMoveMax;
        TParamRT<f32> mSinkRecover;
        TParamRT<f32> mSinkJumpRateMin;
        TParamRT<f32> mSinkJumpRateMax;
        TParamRT<f32> mSinkPumpLimit;
        TParamRT<f32> mSinkDmgDepth;
        TParamRT<f32> mFireHeight;
        TParamRT<s16> mDizzySlipCtMax;
        TParamRT<s16> mDizzyWalkCtMax;
        TParamRT<s16> mDizzyAngleY;
        TParamRT<f32> mDizzyAngleRate;
        TParamRT<f32> mDizzyPowerRate;
        TParamRT<f32> mDizzyPower;
        TParamRT<s16> mFireInvincibleTime;
        TParamRT<s16> mFootEraseTimes;
        TParamRT<f32> mFootEraseSize;
        TParamRT<f32> mFootEraseFront;
    };

    struct TDirtyParams : public TParams
    {
        TDirtyParams()
            : mIncRunning(0.1f, "mIncRunning"),
              mIncCatching(0.3f, "mIncCatching"),
              mIncSlipping(0.2f, "mIncSlipping"),
              mDecSwimming(0.5f, "mDecSwimming"),
              mDecWaterHit(0.2f, "mDecWaterHit"),
              mDecRotJump(0.1f, "mDecRotJump"),
              mBrakeStartValSlip(0.99f, "mBrakeStartValSlip"),
              mBrakeStartValRun(0.98f, "mBrakeStartValRun"),
              mDirtyTimeSlip(600, "mDirtyTimeSlip"),
              mDirtyTimeRun(600, "mDirtyTimeRun"),
              mPolSizeSlip(200.0f, "mPolSizeSlip"),
              mPolSizeRun(80.0f, "mPolSizeRun"),
              mPolSizeFootPrint(200.0f, "mPolSizeFootPrint"),
              mPolSizeJump(200.0f, "mPolSizeJump"),
              mSlopeAngle(0.99f, "mSlopeAngle"),
              mDirtyMax(200.0f, "mDirtyMax"),
              mSlipAnmSpeed(3.0f, "mSlipAnmSpeed"),
              mSlipRunSp(0.01f, "mSlipRunSp"),
              mSlipCatchSp(0.01f, "mSlipCatchSp"),
              mSlipRotate(100, "mSlipRotate"),
              mSlipCatchRotate(100, "mSlipCatchRotate"),
              mBrakeSlipNoPollute(0.98f, "mBrakeSlipNoPollute"),
              mFogTimeYellow(240, "mFogTimeYellow"),
              mFogTimeRed(600, "mFogTimeRed")
              {
            load("/Mario/Dirty.prm");
        }

        TParamRT<f32> mIncRunning;
        TParamRT<f32> mIncCatching;
        TParamRT<f32> mIncSlipping;
        TParamRT<f32> mDecSwimming;
        TParamRT<f32> mDecWaterHit;
        TParamRT<f32> mDecRotJump;
        TParamRT<f32> mBrakeStartValSlip;
        TParamRT<f32> mBrakeStartValRun;
        TParamRT<s16> mDirtyTimeSlip;
        TParamRT<s16> mDirtyTimeRun;
        TParamRT<f32> mPolSizeSlip;
        TParamRT<f32> mPolSizeRun;
        TParamRT<f32> mPolSizeFootPrint;
        TParamRT<f32> mPolSizeJump;
        TParamRT<f32> mSlopeAngle;
        TParamRT<f32> mDirtyMax;
        TParamRT<f32> mSlipAnmSpeed;
        TParamRT<f32> mSlipRunSp;
        TParamRT<f32> mSlipCatchSp;
        TParamRT<s16> mSlipRotate;
        TParamRT<s16> mSlipCatchRotate;
        TParamRT<f32> mBrakeSlipNoPollute;
        TParamRT<s16> mFogTimeYellow;
        TParamRT<s16> mFogTimeRed;
    };

    struct TMotorParams : public TParams
    {
        TMotorParams()
            : mMotorReturn(25, "mMotorReturn"),
              mMotorTrample(8, "mMotorTrample"),
              mMotorHipDrop(15, "mMotorHipDrop"),
              mMotorWall(6, "mMotorWall")
        {
            load("/Mario/MarioMotor.prm");
        }

        TParamRT<s16> mMotorReturn;
        TParamRT<s16> mMotorTrample;
        TParamRT<s16> mMotorHipDrop;
        TParamRT<s16> mMotorWall;
    };

    struct TParticleParams : public TParams
    {
        TParticleParams()
            : mMeltInWaterMax(0.5f, "mMeltInWaterMax"),
              mWaveEmitSpeed(5.0f, "mWaveEmitSpeed"),
              mWaveAlphaDec(5, "mWaveAlphaDec"),
              mBubbleDepth(10.0f, "mBubbleDepth"),
              mBodyBubbleSpMin(0.0f, "mBodyBubbleSpMin"),
              mBodyBubbleSpMax(40.0f, "mBodyBubbleSpMax"),
              mBodyBubbleEmitMin(0.0f, "mBodyBubbleEmitMin"),
              mBodyBubbleEmitMax(0.5f, "mBodyBubbleEmitMax"),
              mBubbleToRipple(0.3f, "mBubbleToRipple"),
              mToroccoWind(0.001f, "mToroccoWind"),
              mToroccoSpark(0.001f, "mToroccoSpark")
        {
            load("/Mario/MarioParticle.prm");
        }

        TParamRT<f32> mMeltInWaterMax;
        TParamRT<f32> mWaveEmitSpeed;
        TParamRT<s16> mWaveAlphaDec;
        TParamRT<f32> mBubbleDepth;
        TParamRT<f32> mBodyBubbleSpMin;
        TParamRT<f32> mBodyBubbleSpMax;
        TParamRT<f32> mBodyBubbleEmitMin;
        TParamRT<f32> mBodyBubbleEmitMax;
        TParamRT<f32> mBubbleToRipple;
        TParamRT<f32> mToroccoWind;
        TParamRT<f32> mToroccoSpark;
    };

    struct TEffectParams : public TParams
    {
        TEffectParams()
            : mDashInc(1.0f / 3.0f, "mDashInc"),
              mDashDec(1.0f / 6.0f, "mDashDec"),
              mDashMaxBlendInBlur(180, "mDashMaxBlendInBlur"),
              mDashMaxBlendInIris(180, "mDashMaxBlendInIris"),
              mDashBlendScale(0.2f, "mDashBlendScale")
        {
            load("/Mario/MarioEffect.prm");
        }

        TParamRT<f32> mDashInc;
        TParamRT<f32> mDashDec;
        TParamRT<u8> mDashMaxBlendInBlur;
        TParamRT<u8> mDashMaxBlendInIris;
        TParamRT<f32> mDashBlendScale;
    };

    struct TSlipParams : public TParams
    {
        TSlipParams(const char *prm)
            : mSlipFriction(0.9f, "mSlipFriction"),
              mSlopeAcceleUp(0.0f, "mSlopeAcceleUp"),
              mSlopeAcceleDown(0.0f, "mSlopeAcceleDown"),
              mSlideAcceleUp(0.0f, "mSlideAcceleUp"),
              mSlideAcceleDown(0.0f, "mSlideAcceleDown"),
              mSlideStopNormal(15.0f, "mSlideStopNormal"),
              mSlideStopCatch(15.0f, "mSlideStopCatch"),
              mJumpEnable(1, "mJumpEnable"),
              mMissJump(1, "mMissJump"),
              mSlideAngleYSp(512, "mSlideAngleYSp"),
              mStickSlideMult(0.05, "mStickSlideMult")
        {
            load(prm);
        }

        TParamRT<f32> mSlipFriction;
        TParamRT<f32> mSlopeAcceleUp;
        TParamRT<f32> mSlopeAcceleDown;
        TParamRT<f32> mSlideAcceleUp;
        TParamRT<f32> mSlideAcceleDown;
        TParamRT<f32> mSlideStopNormal;
        TParamRT<f32> mSlideStopCatch;
        TParamRT<u8> mJumpEnable;
        TParamRT<u8> mMissJump;
        TParamRT<s16> mSlideAngleYSp;
        TParamRT<f32> mStickSlideMult;
    };

    struct TUpperBodyParams : public TParams
    {
        TUpperBodyParams()
            : mPumpWaitTime(10, "mPumpWaitTime"),
              mPumpAnmSpeed(0.01f, "mPumpAnmSpeed"),
              mHoverHeadAngle(-8192, "mHoverHeadAngle"),
              mFeelDeepHeadAngle(8192, "mFeelDeepHeadAngle"),
              mFrontWallHeadAngle(-8192, "mFrontWallHeadAngle")
        {
            load("/Mario/UpperBody.prm");
        }

        TParamRT<s16> mPumpWaitTime;
        TParamRT<f32> mPumpAnmSpeed;
        TParamRT<s16> mHoverHeadAngle;
        TParamRT<s16> mFeelDeepHeadAngle;
        TParamRT<s16> mFrontWallHeadAngle;
    };

    struct TDmgEnemyParams : public TParams
    {
        TDmgEnemyParams(const char *prm)
            : mDamage(1, "mDamage"),
              mDownType(1, "mDownType"),
              mWaterEmit(0, "mWaterEmit"),
              mMotor(25, "mMotor"),
              mMinSpeed(16.0f, "mMinSpeed"),
              mDirty(0.0f, "mDirty"),
              mInvincibleTime(300, "mInvincibleTime")
        {
            load(prm);
        }

        TParamRT<u8> mDamage;
        TParamRT<u8> mDownType;
        TParamRT<u8> mWaterEmit;
        TParamRT<u8> mMotor;
        TParamRT<f32> mMinSpeed;
        TParamRT<f32> mDirty;
        TParamRT<s16> mInvincibleTime;
    };

    struct TDemoParams : public TParams
    {
        TDemoParams()
            : mWarpInBallsDispTime(6, "mWarpInBallsDispTime"),
              mWarpInBallsTime(70, "mWarpInBallsTime"),
              mWarpInCapturedTime(120, "mWarpInCapturedTime"),
              mWarpInTremble(15.0f, "mWarpInTremble"),
              mWarpInVecBase(0.3f, "mWarpInVecBase"),
              mWarpTransTremble(50.0f, "mWarpTransTremble"),
              mReadRotSp(1024, "mReadRotSp")
        {
            load("/Mario/AutoDemo.prm");
        }

        TParamRT<s16> mWarpInBallsDispTime;
        TParamRT<s16> mWarpInBallsTime;
        TParamRT<s16> mWarpInCapturedTime;
        TParamRT<f32> mWarpInTremble;
        TParamRT<f32> mWarpInVecBase;
        TParamRT<f32> mWarpTransTremble;
        TParamRT<s16> mReadRotSp;
    };

    struct TSoundParams : public TParams
    {
        TSoundParams()
            : mStartFallVoiceSpeed(60.0f, "mStartFallVoiceSpeed")
        {
            load("/Mario/Sound.prm");
        }

        TParamRT<f32> mStartFallVoiceSpeed;
    };

    struct TOptionParams : public TParams
    {
        TOptionParams()
            : mZ(-1000.0f, "mZ"),
              mXMin(846.0f, "mXMin"),
              mXMax(2000.0f, "mXMax")
        {
            load("/Mario/Option.prm");
        }

        TParamRT<f32> mZ;
        TParamRT<f32> mXMin;
        TParamRT<f32> mXMax;
    };

    enum State
    {
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

    u32 mActionState;   //0x0074
    u32 mJumpingState;  //0x0078
    u32 mState;         //0x007C
    u32 mPrevState;     //0x0080
    u16 mSubState;      //0x0084
    u16 mSubStateTimer; //0x0086
    u32 _88;
    f32 mBaseAcceleration;        //0x008C
    u16 mAccelerationDirection;   //0x0090
    u16 _92;                      // padding?
    JGeometry::TVec3<u16> mAngle; //0x0094
    JGeometry::TVec3<u16> _9A;
    u16 _A0;
    JGeometry::TVec3<f32> mSpeed; //0x00A4
    f32 mForwardSpeed;            //0x00B0
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
    TBGCheckData *mWallTriangle;      //0x00D8
    TBGCheckData *mRoofTriangle;      //0x00DC
    TBGCheckData *mFloorTriangle;     //0x00E0
    TBGCheckData *mFloorTriangleCopy; //0x00E4
    f32 mCeilingAbove;                //0x00E8
    f32 mFloorBelow;                  //0x00EC
    f32 mWaterHeight;                 //0x00F0
    u16 _F4;
    u16 _F6;
    u16 _F8;
    u16 _FA;
    u16 _FC;
    u16 _FE;
    u16 _100;
    f32 _104;
    TMarioControllerWork *mControllerWork; //0x0108
    u32 _10C;                              //unknown
    u32 _110;                              //unknown
    u32 _114;                              //unknown

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

    u16 mHealth; //0x0120
    u16 _122;    //0x0122
    u16 _124;
    u16 _126;
    u16 _128;
    f32 mWaterHealth;    //0x012C
    f32 mMaxWaterHealth; //0x0130
    u32 _134;            // unknown
    u32 _138;            // unknown
    u32 _13C;            // unknown
    u32 _140;            // unknown
    u32 _144;
    u32 _148;
    u16 _14C;
    u16 _14E;
    u32 _150; // unknown
    u32 _154;
    u32 _158;
    f32 mCollisionXZSize; //0x015C
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
    u32 mInitialWater;                   //0x0298
    JGeometry::TVec3<f32> _29C;
    JGeometry::TVec3<f32> mLastGroundedPos; //0x02A8
    u32 _2B4;
    u16 _2B8;
    u16 mOBTimer; //0x02BA
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
    TTakeActor *mGrabTarget; //0x0384
    u8 _388;
    u8 mSurfGessoID;       //0x0389
    f32 mHolderHeightDiff; //0x038C
    u32 _390;
    J3DDrawBuffer *mDrawBufferA; //0x0394
    J3DDrawBuffer *mDrawBufferB; //0x0398
    u32 _39C;
    u32 _3A0;
    u32 _3A4;                  // unknown
    M3UModelMario *mModelData; //0x03A8
    u32 _3AC;                  // unknown
    u32 _3B0;
    u32 _3B4;
    u32 _3B8;
    u32 _3BC;
    u32 _3C0;                // unknown
    u8 _3C4;                 // padding?
    u8 mBindBoneIDArray[12]; //0x03C5
    u8 _3D1;                 // padding?
    u8 _3D2;                 // padding?
    u8 _3D3;                 // padding?
    u16 _3D4;
    u16 _3D6;
    f32 _3D8;
    f32 _3DC;
    TMarioCap *mCap;   //0x03E0
    TWaterGun *mFludd; //0x03E4
    u32 _3E8;
    f32 _3EC;
    TYoshi *mYoshi;   //0x03F0
    void *mSurfGesso; //0x03F4
    void *_3F8;
    void *_3FC;
    void *_400; //used before for custom pointer
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
    THitActor _474;
    u32 _4DC;
    u32 _4E0;
    u32 _4E4;
    u32 _4E8;
    u8 _4EC;
    u8 _4ED;
    f32 _4F0;
    f32 _4F4;
    f32 _4F8;
    TMarioGamePad *mController;     //0x04FC
    TMarioSoundValues mSoundValues; //0x0500
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
    TDeParams mDeParams;                            //0x0574
    TBodyAngleParams mBodyAngleFreeParams;          //0x09A0
    TBodyAngleParams mBodyAngleWaterGunParams;      //0x0A20
    TAttackParams mPunchFenceParams;                //0x0AA0
    TAttackParams mKickRoofParams;                  //0x0AD0
    TJumpParams mJumpParams;                        //0x0B00
    TRunParams mRunParams;                          //0x0EA0
    TSwimParams mSwimParams;                        //0x104C
    THangFenceParams mHangFenceParams;              //0x1298
    THangRoofParams mHangRoofParams;                //0x1318
    TWireParams mWireParams;                        //0x1334
    TPullParams mPullBGBeakParams;                  //0x147C
    TPullParams mPullBGTentacleParams;              //0x14D4
    TPullParams mPullBGFireWanWanBossTailParams;    //0x152C
    TPullParams mPullFireWanWanTailParams;          //0x1584
    TSurfingParams mSurfingWaterRedParams;          //0x1648
    TSurfingParams mSurfingGroundRedParams;         //0x181C
    TSurfingParams mSurfingWaterYellowParams;       //0x19F0
    TSurfingParams mSurfingGroundYellowParams;      //0x1BC4
    TSurfingParams mSurfingWaterGreenParams;        //0x1D98
    TSurfingParams mSurfingGroundGreenParams;       //0x1F6C
    THoverParams mHoverParams;                      //0x2140
    TDivingParams mDivingParams;                    //0x2184
    TYoshiParams mYoshiParams;                      //0x21F0
    TWaterEffectParams mWaterEffectParams;          //0x22AC
    TControllerParams mControllerParams;            //0x2340
    TGraffitoParams mGraffitoParams;                //0x2410
    TDirtyParams mDirtyParams;                      //0x25BC
    TMotorParams mMotorParams;                      //0x27A4
    TParticleParams mParticleParams;                //0x27FC
    TEffectParams mEffectParams;                    //0x28E0
    TSlipParams mSlipNormalParams;                  //0x294C
    TSlipParams mSlipOilParams;                     //0x2A30
    TSlipParams mSlipAllParams;                     //0x2B14
    TSlipParams mSlipAllSliderParams;               //0x2BF8
    TSlipParams mSlip45Params;                      //0x2CDC
    TSlipParams mSlipWaterSlopeParams;              //0x2DC0
    TSlipParams mSlipWaterGroundParams;             //0x2EA4
    TSlipParams mSlipYoshiParams;                   //0x2F88
    TUpperBodyParams mUpperBodyParams;              //0x306C
    TDmgEnemyParams mDmgEnemyCommonParams;          //0x30D8
    TDmgEnemyParams mDmgHamukuriParams;             //0x316C
    TDmgEnemyParams mDmgNamekuriParams;             //0x3200
    TDmgEnemyParams mDmgHinokuriParams;             //0x3294
    TDmgEnemyParams mDmgFireParams;                 //0x3328
    TDmgEnemyParams mDmgBGTentacleParams;           //0x33BC
    TDmgEnemyParams mDmgBossEelParams;              //0x3450
    TDmgEnemyParams mDmgHanachanBossParams;         //0x34E4
    TDmgEnemyParams mDmgPoihanaParams;              //0x3578
    TDmgEnemyParams mDmgKillerParams;               //0x360C
    TDmgEnemyParams mDmgLampTrapIronParams;         //0x36A0
    TDmgEnemyParams mDmgLampTrapSpikeParams;        //0x3734
    TDmgEnemyParams mDmgEnemyMarioParams;           //0x37C8
    TDmgEnemyParams mDmgCannotBreathParams;         //0x385C
    TDmgEnemyParams mDmgGraffitoFireParams;         //0x38F0
    TDmgEnemyParams mDmgGraffitoPoisonParams;       //0x3984
    TDmgEnemyParams mDmgGraffitoElecParams;         //0x3A18
    TDmgEnemyParams mDmgGraffitoLavaParams;         //0x3AAC
    TDmgEnemyParams mDmgGraffitoWaterSurfaceParams; //0x3B40
    TDmgEnemyParams mDmgMapCode0Params;             //0x3BD4
    TDmgEnemyParams mDmgMapCode1Params;             //0x3C68
    TDmgEnemyParams mDmgMapCode2Params;             //0x3CFC
    TDmgEnemyParams mDmgMapCode3Params;             //0x3D90
    TDmgEnemyParams mDmgMapCode4Params;             //0x3E24
    TDmgEnemyParams mDmgMapCode5Params;             //0x3EB8
    TDmgEnemyParams mDmgMapCode6Params;             //0x3F4C
    TDmgEnemyParams mDmgMapCode7Params;             //0x3FE0
    TDmgEnemyParams mDmgMapCode8Params;             //0x4074
    TDmgEnemyParams mDmgMapCode9Params;             //0x4108
    TDemoParams mDemoParams;                        //0x419C
    TSoundParams mSoundParams;                      //0x4230
    TOptionParams mOptionParams;                    //0x424C
};