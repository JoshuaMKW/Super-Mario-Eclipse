#include "params/MarioParams.hxx"
#include "OS.h"
#include "libs/sAssert.hxx"
#include "libs/sLogging.hxx"
#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/camera/PolarSubCamera.hxx"
#include "string.h"

using namespace SME::Class;

TPlayerData::TPlayerData(TMario *player, CPolarSubCamera *camera, bool isMario)
    : mPlayer(player), mCamera(camera), mIsEMario(!isMario),
      mPlayerID(SME::Enum::Player::MARIO), mCurJump(0), mIsClimbTired(false),
      mPrevCollisionType(0), mCollisionTimer(0), mClimbTiredTimer(0),
      mSlideSpeedMultiplier(1.0f), mYoshiWaterSpeed(0.0f, 0.0f, 0.0f),
      mDefaultAttrs(player) {

  mParams = new TPlayerParams();

  mFluddHistory.mHadFludd = false;
  mFluddHistory.mMainNozzle = TWaterGun::Spray;
  mFluddHistory.mSecondNozzle = TWaterGun::Hover;
  mFluddHistory.mWaterLevel = 0;

  mCollisionFlags.mIsAirborn = false;
  mCollisionFlags.mIsCollisionUsed = false;
  mCollisionFlags.mIsDisableInput = false;
  mCollisionFlags.mIsFaceUsed = false;
  mCollisionFlags.mIsSpinBounce = false;

  if (isMario && loadPrm("/sme.prm")) {
    SME_DEBUG_LOG("Custom character params loaded!\n");
  } else {
    SME_DEBUG_LOG("Default character params loaded!\n");
  }

  mCanUseFludd = mParams->mCanUseFludd.get();

  if (mParams->mPlayerHasGlasses.get() && player->mCap)
    reinterpret_cast<u16 *>(player->mCap)[2] |= 0b100;

  scalePlayerAttrs(mParams->mSizeMultiplier.get());
}

static u32 Timer = 0;

void TPlayerData::scalePlayerAttrs(f32 scale) {
  scale = Max(scale, 0.0f);

  JGeometry::TVec3<f32> size(1.0f, 1.0f, 1.0f);
  size.scale(scale);

  mPlayer->JSGSetScaling(reinterpret_cast<Vec &>(size));
  mPlayer->mModelData->mModel->mScale.set(size);

  mDefaultAttrs.applyHistoryTo(const_cast<TMario *>(getPlayer()));

#define SCALE_PARAM(param, scale) param.set(param.get() * scale)

  const TPlayerParams *params = getParams();

  const f32 yoshiAgility =
      SME::Util::Math::sigmoidCurve(size.y, 0.0f, 1.2f, 1.321887582486f, -5.0f);

  f32 factor = Util::Math::scaleLinearAtAnchor<f32>(scale, 0.5f, 1.0f);
  f32 speedMultiplier = params->mSpeedMultiplier.get();
  f32 jumpMultiplier = params->mBaseJumpMultiplier.get();
  f32 gravityMultiplier = params->mGravityMultiplier.get();
  if (onYoshi__6TMarioCFv(mPlayer)) {
    factor = 1.0f;
    scale = 1.0f;
    speedMultiplier = 1.0f;
    jumpMultiplier = 1.0f;
  }

  if (Timer++ % 1000 == 0)
    SME_DEBUG_LOG("{\n"
                  "  Factor:        %.04f\n"
                  "  Scale:         %.04f\n"
                  "  Speed:         %.04f\n"
                  "  Jump:          %.04f\n"
                  "}\n",
                  factor, scale, speedMultiplier, jumpMultiplier);

  SCALE_PARAM(mPlayer->mDeParams.mRunningMax, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mDeParams.mDashMax, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mDeParams.mShadowSize, scale);
  SCALE_PARAM(mPlayer->mDeParams.mHoldRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mDamageRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mDamageHeight, scale);
  SCALE_PARAM(mPlayer->mDeParams.mAttackHeight, scale);
  SCALE_PARAM(mPlayer->mDeParams.mTrampleRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mPushupRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mPushupHeight, scale);
  SCALE_PARAM(mPlayer->mDeParams.mHipdropRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mQuakeRadius, scale);
  SCALE_PARAM(mPlayer->mDeParams.mQuakeHeight, scale);
  SCALE_PARAM(mPlayer->mDeParams.mJumpWallHeight, factor);
  SCALE_PARAM(mPlayer->mDeParams.mThrowPower,
              factor * params->mThrowPowerMultiplier.get());
  SCALE_PARAM(mPlayer->mDeParams.mFeelDeep, factor);
  SCALE_PARAM(mPlayer->mDeParams.mDamageFallHeight, factor);
  SCALE_PARAM(mPlayer->mDeParams.mClashSpeed, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mDeParams.mSleepingCheckDist, factor);
  SCALE_PARAM(mPlayer->mDeParams.mSleepingCheckHeight, factor);
  SCALE_PARAM(mPlayer->mPunchFenceParams.mRadius, factor);
  SCALE_PARAM(mPlayer->mPunchFenceParams.mHeight, factor);
  SCALE_PARAM(mPlayer->mKickRoofParams.mRadius, factor);
  SCALE_PARAM(mPlayer->mKickRoofParams.mHeight, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mGravity, factor * gravityMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mSpinJumpGravity,
              factor * gravityMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mJumpSpeedAccelControl,
              factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mPopUpSpeedY, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mJumpingMax, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mFenceSpeed, factor * speedMultiplier);
  //SCALE_PARAM(mPlayer->mJumpParams.mFireBackVelocity, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mBroadJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBroadJumpForceY, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mRotateJumpForceY, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mBackJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBackJumpForceY, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mHipAttackSpeedY,
              factor * gravityMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mSuperHipAttackSpeedY,
              factor * gravityMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mRotBroadJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mRotBroadJumpForceY,
              factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mSecJumpForce, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mUltraJumpForce, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mTurnJumpForce, factor * jumpMultiplier);
  SCALE_PARAM(mPlayer->mJumpParams.mTriJumpEnableSp, scale);
  SCALE_PARAM(mPlayer->mJumpParams.mValleyDepth, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mTremblePower, 1.0f / factor);
  SCALE_PARAM(mPlayer->mJumpParams.mTrembleTime,
              static_cast<s16>(1.0f / factor));
  SCALE_PARAM(mPlayer->mJumpParams.mGetOffYoshiY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mSuperHipAttackCt,
              static_cast<s16>(1.0f / factor));
  SCALE_PARAM(mPlayer->mRunParams.mMaxSpeed, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mRunParams.mAddBase, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mRunParams.mDecBrake, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mRunParams.mSoft2Walk, factor);
  SCALE_PARAM(mPlayer->mRunParams.mWalk2Soft, factor);
  SCALE_PARAM(mPlayer->mRunParams.mSoftStepAnmMult, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mRunAnmSpeedMult, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mMotBlendWalkSp, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mMotBlendRunSp, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mSwimDepth, factor);
  SCALE_PARAM(mPlayer->mRunParams.mTurnNeedSp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mStartSp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mMoveSp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mGravity, factor * gravityMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mWaitBouyancy, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mMoveBouyancy, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mCanJumpDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mEndDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mFloatHeight, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mRush, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleSpeedUp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleJumpUp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mFloatUp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mWaterLevelCheckHeight, 1.0f / scale);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleDown, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mSwimParams.mCanBreathDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mWaitSinkSpeed, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mAirDec,
              1.0f / params->mUnderwaterHealthMultiplier.get());
  SCALE_PARAM(mPlayer->mSwimParams.mAirDecDive,
              1.0f / params->mUnderwaterHealthMultiplier.get());
  SCALE_PARAM(mPlayer->mHangFenceParams.mMoveSp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mHangFenceParams.mDescentSp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mPullBGBeakParams.mPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGBeakParams.mPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullBGBeakParams.mOilPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGBeakParams.mOilPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullBGTentacleParams.mPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGTentacleParams.mPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullBGTentacleParams.mOilPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGTentacleParams.mOilPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullBGFireWanWanBossTailParams.mPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGFireWanWanBossTailParams.mPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullBGFireWanWanBossTailParams.mOilPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullBGFireWanWanBossTailParams.mOilPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullFireWanWanTailParams.mPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullFireWanWanTailParams.mPullRateH, factor);
  SCALE_PARAM(mPlayer->mPullFireWanWanTailParams.mOilPullRateV, factor);
  SCALE_PARAM(mPlayer->mPullFireWanWanTailParams.mOilPullRateH, factor);
  SCALE_PARAM(mPlayer->mDivingParams.mGravity, factor);
  SCALE_PARAM(mPlayer->mYoshiParams.mRunYoshiMult, yoshiAgility);
  SCALE_PARAM(mPlayer->mYoshiParams.mJumpYoshiMult, yoshiAgility);
  SCALE_PARAM(mPlayer->mYoshiParams.mRotYoshiMult, yoshiAgility);
  SCALE_PARAM(mPlayer->mWaterEffectParams.mJumpIntoMinY, factor);
  SCALE_PARAM(mPlayer->mWaterEffectParams.mJumpIntoMaxY, factor);
  SCALE_PARAM(mPlayer->mWaterEffectParams.mJumpIntoScaleMin, factor);
  SCALE_PARAM(mPlayer->mWaterEffectParams.mJumpIntoScaleWidth, factor);
  SCALE_PARAM(mPlayer->mWaterEffectParams.mRunningRippleDepth, factor);
  SCALE_PARAM(mPlayer->mGraffitoParams.mSinkHeight, factor);
  SCALE_PARAM(mPlayer->mGraffitoParams.mSinkMoveMin, factor);
  SCALE_PARAM(mPlayer->mGraffitoParams.mSinkMoveMax, factor);
  SCALE_PARAM(mPlayer->mGraffitoParams.mSinkRecover, factor);
  SCALE_PARAM(mPlayer->mGraffitoParams.mFootEraseSize, scale);
  SCALE_PARAM(mPlayer->mDirtyParams.mPolSizeSlip, scale);
  SCALE_PARAM(mPlayer->mDirtyParams.mPolSizeRun, scale);
  SCALE_PARAM(mPlayer->mDirtyParams.mPolSizeFootPrint, scale);
  SCALE_PARAM(mPlayer->mDirtyParams.mPolSizeJump, scale);
  SCALE_PARAM(mPlayer->mDirtyParams.mSlipAnmSpeed, 1 / factor);
  SCALE_PARAM(mPlayer->mDirtyParams.mSlipRunSp, factor * speedMultiplier);
  SCALE_PARAM(mPlayer->mDirtyParams.mSlipCatchSp, factor * speedMultiplier);

#undef SCALE_PARAM
}

void TPlayerData::setPlayer(TMario *player) {
  mPlayer = player;
  mDefaultAttrs.recordFrom(player);
}

bool TPlayerData::loadPrm(const char *prm = "/sme.prm") {
  JKRArchive *archive = JKRFileLoader::getVolume("mario");
  SME_DEBUG_ASSERT(archive, "Mario archive could not be located!");

  void *resource = archive->getResource(prm);
  if (resource) {
    JSUMemoryInputStream stream(resource, archive->getResSize(resource));
    mParams->load(stream);
    return true;
  }
  return false;
}

bool TPlayerData::loadPrm(JSUMemoryInputStream &stream) {
  mParams->load(stream);
  return true;
}

bool TPlayerData::canUseNozzle(TWaterGun::NozzleType nozzle) const {
  switch (nozzle) {
  case TWaterGun::NozzleType::Spray:
    return mParams->mSprayNozzleUsable.get();
  case TWaterGun::NozzleType::Rocket:
    return mParams->mRocketNozzleUsable.get();
  case TWaterGun::NozzleType::Underwater:
    return mParams->mUnderwaterNozzleUsable.get();
  case TWaterGun::NozzleType::Yoshi:
    return mParams->mYoshiNozzleUsable.get();
  case TWaterGun::NozzleType::Hover:
    return mParams->mHoverNozzleUsable.get();
  case TWaterGun::NozzleType::Turbo:
    return mParams->mTurboNozzleUsable.get();
  case TWaterGun::NozzleType::Sniper:
    return mParams->mSniperNozzleUsable.get();
  default:
    return false;
  }
}

u8 TPlayerData::getNozzleBoneID(TWaterGun::NozzleType nozzle) const {
  switch (nozzle) {
  case TWaterGun::NozzleType::Spray:
    return mParams->mSprayNozzleBoneID.get();
  case TWaterGun::NozzleType::Rocket:
    return mParams->mRocketNozzleBoneID.get();
  case TWaterGun::NozzleType::Underwater:
    return mParams->mUnderwaterNozzleBoneID.get();
  case TWaterGun::NozzleType::Yoshi:
    return mParams->mYoshiNozzleBoneID.get();
  case TWaterGun::NozzleType::Hover:
    return mParams->mHoverNozzleBoneID.get();
  case TWaterGun::NozzleType::Turbo:
    return mParams->mTurboNozzleBoneID.get();
  case TWaterGun::NozzleType::Sniper:
    return mParams->mSniperNozzleBoneID.get();
  default:
    return false;
  }
}

const char *TPlayerData::getPlayerName() const {
  switch (getPlayerID()) {
  case SME::Enum::Player::LUIGI:
    return "Luigi";
  case SME::Enum::Player::IL_PIANTISSIMO:
    return "Il Piantissimo";
  case SME::Enum::Player::SHADOW_MARIO:
    return "Shadow Mario";
  case SME::Enum::Player::DRY_BONES:
    return "Dry Bones";
  case SME::Enum::Player::MARIO:
  default:
    return "Mario";
  }
}

void TPlayerData::ParamHistory::applyHistoryTo(TMario *player) {
  SME_ASSERT(player != nullptr, "Can't apply param history to a nullptr");
  player->mDeParams = mDeParams;
  player->mBodyAngleFreeParams = mBodyAngleFreeParams;
  player->mBodyAngleWaterGunParams = mBodyAngleWaterGunParams;
  player->mPunchFenceParams = mPunchFenceParams;
  player->mKickRoofParams = mKickRoofParams;
  player->mJumpParams = mJumpParams;
  player->mRunParams = mRunParams;
  player->mSwimParams = mSwimParams;
  player->mHangFenceParams = mHangFenceParams;
  player->mHangRoofParams = mHangRoofParams;
  player->mWireParams = mWireParams;
  player->mPullBGBeakParams = mPullBGBeakParams;
  player->mPullBGTentacleParams = mPullBGTentacleParams;
  player->mPullBGFireWanWanBossTailParams = mPullBGFireWanWanBossTailParams;
  player->mPullFireWanWanTailParams = mPullFireWanWanTailParams;
  player->mHoverParams = mHoverParams;
  player->mDivingParams = mDivingParams;
  player->mYoshiParams = mYoshiParams;
  player->mWaterEffectParams = mWaterEffectParams;
  player->mControllerParams = mControllerParams;
  player->mGraffitoParams = mGraffitoParams;
  player->mDirtyParams = mDirtyParams;
  player->mMotorParams = mMotorParams;
  player->mParticleParams = mParticleParams;
  player->mEffectParams = mEffectParams;
  player->mSlipNormalParams = mSlipNormalParams;
  player->mSlipOilParams = mSlipOilParams;
  player->mSlipAllParams = mSlipAllParams;
  player->mSlipAllSliderParams = mSlipAllSliderParams;
  player->mSlip45Params = mSlip45Params;
  player->mSlipWaterSlopeParams = mSlipWaterSlopeParams;
  player->mSlipWaterGroundParams = mSlipWaterGroundParams;
  player->mSlipYoshiParams = mSlipYoshiParams;
  player->mUpperBodyParams = mUpperBodyParams;
}

TPlayerData::ParamHistory::ParamHistory()
    : mDeParams(), mBodyAngleFreeParams("/Mario/BodyAngleFree.prm"),
      mBodyAngleWaterGunParams("/Mario/BodyAngleWaterGun.prm"),
      mPunchFenceParams("/Mario/AttackFencePunch.prm"),
      mKickRoofParams("/Mario/AttackKickRoof.prm"), mJumpParams(), mRunParams(),
      mSwimParams(), mHangFenceParams(), mHangRoofParams(), mWireParams(),
      mPullBGBeakParams("/Mario/PullParamBGBeak.prm"),
      mPullBGTentacleParams("/Mario/PullParamBGTentacle.prm"),
      mPullBGFireWanWanBossTailParams(
          "/Mario/PullParamBGFireWanWanBossTail.prm"),
      mPullFireWanWanTailParams("/Mario/PullParamFireWanWanTail.prm"),
      mHoverParams(), mDivingParams(), mYoshiParams(), mWaterEffectParams(),
      mControllerParams(), mGraffitoParams(), mDirtyParams(), mMotorParams(),
      mParticleParams(), mEffectParams(),
      mSlipNormalParams("/Mario/SlipParamNormal.prm"),
      mSlipOilParams("/Mario/SlipParamOil.prm"),
      mSlipAllParams("/Mario/SlipParamAll.prm"),
      mSlipAllSliderParams("/Mario/SlipParamAll_Slider.prm"),
      mSlip45Params("/Mario/SlipParam45.prm"),
      mSlipWaterSlopeParams("/Mario/SlipParamWaterSlope.prm"),
      mSlipWaterGroundParams("/Mario/SlipParamWaterGround.prm"),
      mSlipYoshiParams("/Mario/SlipParamYoshi.prm"), mUpperBodyParams() {}

TPlayerData::ParamHistory::ParamHistory(TMario *player) : ParamHistory() {
  recordFrom(player);
}

void TPlayerData::ParamHistory::recordFrom(TMario *player) {
  SME_ASSERT(player != nullptr, "Can't record param history from a nullptr");
  mDeParams = player->mDeParams;
  mBodyAngleFreeParams = player->mBodyAngleFreeParams;
  mBodyAngleWaterGunParams = player->mBodyAngleWaterGunParams;
  mPunchFenceParams = player->mPunchFenceParams;
  mKickRoofParams = player->mKickRoofParams;
  mJumpParams = player->mJumpParams;
  mRunParams = player->mRunParams;
  mSwimParams = player->mSwimParams;
  mHangFenceParams = player->mHangFenceParams;
  mHangRoofParams = player->mHangRoofParams;
  mWireParams = player->mWireParams;
  mPullBGBeakParams = player->mPullBGBeakParams;
  mPullBGTentacleParams = player->mPullBGTentacleParams;
  mPullBGFireWanWanBossTailParams = player->mPullBGFireWanWanBossTailParams;
  mPullFireWanWanTailParams = player->mPullFireWanWanTailParams;
  mHoverParams = player->mHoverParams;
  mDivingParams = player->mDivingParams;
  mYoshiParams = player->mYoshiParams;
  mWaterEffectParams = player->mWaterEffectParams;
  mControllerParams = player->mControllerParams;
  mGraffitoParams = player->mGraffitoParams;
  mDirtyParams = player->mDirtyParams;
  mMotorParams = player->mMotorParams;
  mParticleParams = player->mParticleParams;
  mEffectParams = player->mEffectParams;
  mSlipNormalParams = player->mSlipNormalParams;
  mSlipOilParams = player->mSlipOilParams;
  mSlipAllParams = player->mSlipAllParams;
  mSlipAllSliderParams = player->mSlipAllSliderParams;
  mSlip45Params = player->mSlip45Params;
  mSlipWaterSlopeParams = player->mSlipWaterSlopeParams;
  mSlipWaterGroundParams = player->mSlipWaterGroundParams;
  mSlipYoshiParams = player->mSlipYoshiParams;
  mUpperBodyParams = player->mUpperBodyParams;
}

void TPlayerData::ParamHistory::reset() { *this = ParamHistory(); }
