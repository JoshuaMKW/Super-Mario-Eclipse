#pragma once

#include "mem.h"
#include "types.h"

#include "funcs.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "sms/actor/Mario.hxx"

namespace SME::Class {

struct TCustomParams : public TParams {
  TCustomParams()
      : mMaxJumps(1, "mMaxJumps"), mCanRideYoshi(true, "mCanRideYoshi"),
        mCanUseFludd(true, "mCanUseFludd"),
        mPlayerHasHelmet(false, "mPlayerHasHelmet"),
        mPlayerHasGlasses(false, "mPlayerHasGlasses"),
        mPlayerHasShirt(false, "mPlayerHasShirt"),
        mCanCleanSeals(false, "mCanCleanSeals"),
        mGoopAffected(true, "mGoopAffected"),
        mCanHoldNPCs(false, "mCanHoldNPCs"),
        mCanClimbWalls(false, "mCanClimbWalls"),
        mSizeMultiplier(1.0f, "mSizeMultiplier"),
        mMultiJumpMultiplier(0.875f, "mMultiJumpMultiplier"),
        mMultiJumpFSpeedMulti(0.9f, "mMultiJumpFSpeedMulti"),
        mThrowPowerMultiplier(1.0f, "mThrowPowerMultiplier"),
        mSprayNozzleUsable(true, "mSprayNozzleUsable"),
        mRocketNozzleUsable(true, "mRocketNozzleUsable"),
        mUnderwaterNozzleUsable(true, "mUnderwaterNozzleUsable"),
        mYoshiNozzleUsable(true, "mYoshiNozzleUsable"),
        mHoverNozzleUsable(true, "mHoverNozzleUsable"),
        mTurboNozzleUsable(true, "mTurboNozzleUsable"),
        mSniperNozzleUsable(true, "mSniperNozzleUsable"),
        mSprayNozzleBoneID(14, "mSprayNozzleBoneID"),
        mRocketNozzleBoneID(14, "mRocketNozzleBoneID"),
        mUnderwaterNozzleBoneID(14, "mUnderwaterNozzleBoneID"),
        mYoshiNozzleBoneID(14, "mYoshiNozzleBoneID"),
        mHoverNozzleBoneID(14, "mHoverNozzleBoneID"),
        mTurboNozzleBoneID(14, "mTurboNozzleBoneID"),
        mSniperNozzleBoneID(14, "mSniperNozzleBoneID"),
        mWaterColor(JUtility::TColor{60, 70, 120, 20}, "mSniperNozzleBoneID"),
        mCleaningType(FluddCleanType::CLEAN, "mFluddCleanType"),
        mName("Mario", "mName") {
    load("/Mario/SME.prm");
  }

  enum FluddCleanType : u8 { NONE, CLEAN, GOOP };

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
  TParamRT<char[32]> mName;
};

class PlayerParams {

public:
  struct ParamHistory {
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

  PlayerParams() = delete;
  PlayerParams(TMario *player, bool isMario);

  u8 getMaxJumps() const { return mParams->mMaxJumps.get(); }
  const TCustomParams *getParams() const { return mParams; }
  const TMario *getPlayer() const { return mPlayer; }
  s8 getPlayerID() const { return mPlayerID; }
  const u16 getPlayerKey() const {
    return JDrama::TNameRef::calcKeyCode(getPlayerName());
  }
  const char *getPlayerName() const { return mParams->mName.get(); }
  bool isMario() const { return !mIsEMario; }
  bool isInitialized() const { return mInitialized; }
  void setPlayer(TMario *player) { mPlayer = player; }

  bool canUseNozzle(TWaterGun::NozzleType nozzle) const;
  bool loadPrm();
  bool loadPrm(const char *prm);
  bool loadPrm(TCustomParams &params);
  void resetPlayer() { mDefaultAttrs.applyHistoryTo(mPlayer); };
  void scalePlayerAttrs(f32 scale);

private:
  TMario *mPlayer;
  TCustomParams *mParams;
  bool mIsEMario;
  bool mInitialized;
  s8 mPlayerID;

public:
  u8 mCurJump;
  bool mIsClimbTired;
  u16 mPrevCollisionType;
  s32 mCollisionTimer;

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