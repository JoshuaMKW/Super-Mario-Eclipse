#pragma once

#include "macros.h"
#include "mem.h"
#include "types.h"

#include "funcs.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/camera/PolarSubCamera.hxx"

#include "Player.hxx"

namespace SME::Class {

struct TPlayerParams : public TParams {
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  TPlayerParams()
      : TParams(), CONSTRUCT_PARAM(mMaxJumps, 1),
        CONSTRUCT_PARAM(mCanRideYoshi, true),
        CONSTRUCT_PARAM(mCanUseFludd, true),
        CONSTRUCT_PARAM(mPlayerHasHelmet, false),
        CONSTRUCT_PARAM(mPlayerHasGlasses, false),
        CONSTRUCT_PARAM(mPlayerHasShirt, false),
        CONSTRUCT_PARAM(mCanCleanSeals, false),
        CONSTRUCT_PARAM(mGoopAffected, true),
        CONSTRUCT_PARAM(mCanHoldNPCs, false),
        CONSTRUCT_PARAM(mCanClimbWalls, false),
        CONSTRUCT_PARAM(mSizeMultiplier, 1.0f),
        CONSTRUCT_PARAM(mSpeedMultiplier, 1.0f),
        CONSTRUCT_PARAM(mBaseJumpMultiplier, 1.0f),
        CONSTRUCT_PARAM(mMultiJumpMultiplier, 0.875f),
        CONSTRUCT_PARAM(mMultiJumpFSpeedMulti, 0.9f),
        CONSTRUCT_PARAM(mThrowPowerMultiplier, 1.0f),
        CONSTRUCT_PARAM(mSprayNozzleUsable, true),
        CONSTRUCT_PARAM(mRocketNozzleUsable, true),
        CONSTRUCT_PARAM(mUnderwaterNozzleUsable, true),
        CONSTRUCT_PARAM(mYoshiNozzleUsable, true),
        CONSTRUCT_PARAM(mHoverNozzleUsable, true),
        CONSTRUCT_PARAM(mTurboNozzleUsable, true),
        CONSTRUCT_PARAM(mSniperNozzleUsable, true),
        CONSTRUCT_PARAM(mSprayNozzleBoneID, 14),
        CONSTRUCT_PARAM(mRocketNozzleBoneID, 14),
        CONSTRUCT_PARAM(mUnderwaterNozzleBoneID, 14),
        CONSTRUCT_PARAM(mYoshiNozzleBoneID, 14),
        CONSTRUCT_PARAM(mHoverNozzleBoneID, 14),
        CONSTRUCT_PARAM(mTurboNozzleBoneID, 14),
        CONSTRUCT_PARAM(mSniperNozzleBoneID, 14),
        CONSTRUCT_PARAM(mWaterColor, JUtility::TColor(60, 70, 120, 20)),
        CONSTRUCT_PARAM(mCleaningType, FluddCleanType::CLEAN) {
    load("/Mario/SME.prm");
  }

#undef CONSTRUCT_PARAM

  enum class FluddCleanType : u8 { NONE, CLEAN, GOOP };

  TParamRT<u8> mMaxJumps;
  TParamRT<bool> mCanRideYoshi;
  TParamRT<bool> mCanUseFludd;
  TParamRT<bool> mPlayerHasHelmet;
  TParamRT<bool> mPlayerHasGlasses;
  TParamRT<bool> mPlayerHasShirt;
  TParamRT<bool> mCanCleanSeals;
  TParamRT<bool> mGoopAffected;
  TParamRT<bool> mCanHoldNPCs;
  TParamRT<bool> mCanClimbWalls;
  TParamRT<f32> mSizeMultiplier;
  TParamRT<f32> mSpeedMultiplier;
  TParamRT<f32> mBaseJumpMultiplier;
  TParamRT<f32> mMultiJumpMultiplier;
  TParamRT<f32> mMultiJumpFSpeedMulti;
  TParamRT<f32> mThrowPowerMultiplier;
  TParamRT<bool> mSprayNozzleUsable;
  TParamRT<bool> mRocketNozzleUsable;
  TParamRT<bool> mUnderwaterNozzleUsable;
  TParamRT<bool> mYoshiNozzleUsable;
  TParamRT<bool> mHoverNozzleUsable;
  TParamRT<bool> mTurboNozzleUsable;
  TParamRT<bool> mSniperNozzleUsable;
  TParamRT<u8> mSprayNozzleBoneID;
  TParamRT<u8> mRocketNozzleBoneID;
  TParamRT<u8> mUnderwaterNozzleBoneID;
  TParamRT<u8> mYoshiNozzleBoneID;
  TParamRT<u8> mHoverNozzleBoneID;
  TParamRT<u8> mTurboNozzleBoneID;
  TParamRT<u8> mSniperNozzleBoneID;
  TParamRT<JUtility::TColor> mWaterColor;
  TParamRT<FluddCleanType> mCleaningType;
};

class TPlayerData {

public:
  struct ParamHistory {
    ParamHistory();
    ParamHistory(TMario *);

    bool hasHistory() const { return mHasHistory; }

    void applyHistoryTo(TMario *player);
    void recordFrom(TMario *player);
    void reset();

    TMario::TDeParams mDeParams;
    TMario::TBodyAngleParams mBodyAngleFreeParams;
    TMario::TBodyAngleParams mBodyAngleWaterGunParams;
    TMario::TAttackParams mPunchFenceParams;
    TMario::TAttackParams mKickRoofParams;
    TMario::TJumpParams mJumpParams;
    TMario::TRunParams mRunParams;
    TMario::TSwimParams mSwimParams;
    TMario::THangFenceParams mHangFenceParams;
    TMario::THangRoofParams mHangRoofParams;
    TMario::TWireParams mWireParams;
    TMario::TPullParams mPullBGBeakParams;
    TMario::TPullParams mPullBGTentacleParams;
    TMario::TPullParams mPullBGFireWanWanBossTailParams;
    TMario::TPullParams mPullFireWanWanTailParams;
    TMario::THoverParams mHoverParams;
    TMario::TDivingParams mDivingParams;
    TMario::TYoshiParams mYoshiParams;
    TMario::TWaterEffectParams mWaterEffectParams;
    TMario::TControllerParams mControllerParams;
    TMario::TGraffitoParams mGraffitoParams;
    TMario::TDirtyParams mDirtyParams;
    TMario::TMotorParams mMotorParams;
    TMario::TParticleParams mParticleParams;
    TMario::TEffectParams mEffectParams;
    TMario::TSlipParams mSlipNormalParams;
    TMario::TSlipParams mSlipOilParams;
    TMario::TSlipParams mSlipAllParams;
    TMario::TSlipParams mSlipAllSliderParams;
    TMario::TSlipParams mSlip45Params;
    TMario::TSlipParams mSlipWaterSlopeParams;
    TMario::TSlipParams mSlipWaterGroundParams;
    TMario::TSlipParams mSlipYoshiParams;
    TMario::TUpperBodyParams mUpperBodyParams;

  private:
    bool mHasHistory;
  };

  struct FluddHistory {
    s32 mWaterLevel;
    u8 mMainNozzle;
    u8 mSecondNozzle;
    bool mHadFludd;
  };

  TPlayerData() = delete;
  TPlayerData(TMario *player, CPolarSubCamera *camera, bool isMario);

  CPolarSubCamera *getCamera() const { return mCamera; }
  bool getCanUseFludd() const { return mCanUseFludd; }
  u8 getMaxJumps() const { return mParams->mMaxJumps.get(); }
  u8 getNozzleBoneID(TWaterGun::NozzleType nozzle) const;
  const TPlayerParams *getParams() const { return mParams; }
  const TMario *getPlayer() const { return mPlayer; }
  SME::Enum::Player getPlayerID() const { return mPlayerID; }
  const u16 getPlayerKey() const {
    return JDrama::TNameRef::calcKeyCode(getPlayerName());
  }
  bool isMario() const { return !mIsEMario; }

  void setCamera(CPolarSubCamera *camera);
  void setCanUseFludd(bool enable) { mCanUseFludd = enable; }
  void setPlayer(TMario *player);
  void setPlayerID(SME::Enum::Player id) { mPlayerID = id; }

  bool canUseNozzle(TWaterGun::NozzleType nozzle) const;
  const char *getPlayerName() const;
  bool loadPrm(const char *prm);
  bool loadPrm(JSUMemoryInputStream &stream);
  void resetPlayer() { mDefaultAttrs.applyHistoryTo(mPlayer); };
  void scalePlayerAttrs(f32 scale);

private:
  TMario *mPlayer;
  CPolarSubCamera *mCamera;
  TPlayerParams *mParams;
  bool mIsEMario;
  bool mCanUseFludd;
  SME::Enum::Player mPlayerID;

public:
  u8 mCurJump;
  bool mIsClimbTired;
  u16 mPrevCollisionType;
  s32 mCollisionTimer;
  s32 mClimbTiredTimer;

  struct {
    bool mIsAirborn : 1;
    bool mIsFaceUsed : 1;
    bool mIsSpinBounce : 1;
    bool mIsDisableInput : 1;
    bool mIsCollisionUsed : 1;
  } mCollisionFlags;

  JGeometry::TVec3<f32> mYoshiWaterSpeed;
  FluddHistory mFluddHistory;
  ParamHistory mDefaultAttrs;
};

} // namespace SME::Class