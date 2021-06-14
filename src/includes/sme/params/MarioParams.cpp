#include "params/MarioParams.hxx"
#include "OS.h"
#include "libs/sAssert.hxx"
#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/camera/PolarSubCamera.hxx"
#include "string.h"


using namespace SME::Class;

TPlayerData::TPlayerData(TMario *player, CPolarSubCamera *camera,
                             bool isMario)
    : mPlayer(player), mCamera(camera), mParams(nullptr), mIsEMario(!isMario),
      mInitialized(true), mCanUseFludd(true),
      mPlayerID(SME::Enum::Player::MARIO), mCurJump(0), mIsClimbTired(false),
      mPrevCollisionType(0), mCollisionTimer(0), mClimbTiredTimer(0),
      mYoshiWaterSpeed(0.0f, 0.0f, 0.0f), mDefaultAttrs(player) {

  mFluddHistory.mHadFludd = false;
  mFluddHistory.mMainNozzle = TWaterGun::Spray;
  mFluddHistory.mSecondNozzle = TWaterGun::Hover;
  mFluddHistory.mWaterLevel = 0;

  loadPrm("/sme.prm");

  if (mParams->mPlayerHasGlasses.get())
    wearGlass__6TMarioFv(player);

  scalePlayerAttrs(mParams->mSizeMultiplier.get());
}

void TPlayerData::scalePlayerAttrs(f32 scale) {
  const float factor = (scale * 0.5f) + (1.0f - 0.5f);

  JGeometry::TVec3<f32> size;
  mPlayer->JSGGetScaling(reinterpret_cast<Vec *>(&size));
  size.scale(scale);
  mPlayer->JSGSetScaling(reinterpret_cast<Vec &>(size));
  mPlayer->mModelData->mModel->mScale.scale(scale);

  const f32 yoshiAgility =
      SME::Util::Math::sigmoidCurve(size.y, 0.8f, 5.0f, 1.6f, 5.0f);

#define SCALE_PARAM(param, scale) param.set(param.get() * scale)

  SCALE_PARAM(mPlayer->mDeParams.mRunningMax, factor);
  SCALE_PARAM(mPlayer->mDeParams.mDashMax, factor);
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
  SCALE_PARAM(mPlayer->mDeParams.mThrowPower, factor);
  SCALE_PARAM(mPlayer->mDeParams.mFeelDeep, factor);
  SCALE_PARAM(mPlayer->mDeParams.mDamageFallHeight, factor);
  SCALE_PARAM(mPlayer->mDeParams.mClashSpeed, factor);
  SCALE_PARAM(mPlayer->mDeParams.mSleepingCheckDist, factor);
  SCALE_PARAM(mPlayer->mDeParams.mSleepingCheckHeight, factor);
  SCALE_PARAM(mPlayer->mPunchFenceParams.mRadius, factor);
  SCALE_PARAM(mPlayer->mPunchFenceParams.mHeight, factor);
  SCALE_PARAM(mPlayer->mKickRoofParams.mRadius, factor);
  SCALE_PARAM(mPlayer->mKickRoofParams.mHeight, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mGravity, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mSpinJumpGravity, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mJumpingMax, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mFenceSpeed, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mFireBackVelocity, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBroadJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBroadJumpForceY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mRotateJumpForceY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mPopUpSpeedY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBackJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mBackJumpForceY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mHipAttackSpeedY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mSuperHipAttackSpeedY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mRotBroadJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mRotBroadJumpForceY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mSecJumpForce, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mValleyDepth, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mTremblePower, 1 / factor);
  SCALE_PARAM(mPlayer->mJumpParams.mTrembleTime, static_cast<s16>(1 / factor));
  SCALE_PARAM(mPlayer->mJumpParams.mGetOffYoshiY, factor);
  SCALE_PARAM(mPlayer->mJumpParams.mSuperHipAttackCt,
              static_cast<s16>(1 / factor));
  SCALE_PARAM(mPlayer->mRunParams.mMaxSpeed, factor);
  SCALE_PARAM(mPlayer->mRunParams.mAddBase, factor);
  SCALE_PARAM(mPlayer->mRunParams.mDecBrake, factor);
  SCALE_PARAM(mPlayer->mRunParams.mSoft2Walk, factor);
  SCALE_PARAM(mPlayer->mRunParams.mWalk2Soft, factor);
  SCALE_PARAM(mPlayer->mRunParams.mSoftStepAnmMult, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mRunAnmSpeedMult, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mMotBlendWalkSp, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mMotBlendRunSp, 1 / factor);
  SCALE_PARAM(mPlayer->mRunParams.mSwimDepth, factor);
  SCALE_PARAM(mPlayer->mRunParams.mTurnNeedSp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mStartSp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mMoveSp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mGravity, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mWaitBouyancy, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mMoveBouyancy, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mCanJumpDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mEndDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mFloatHeight, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mRush, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleSpeedUp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleJumpUp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mFloatUp, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mPaddleDown, factor);
  SCALE_PARAM(mPlayer->mSwimParams.mCanBreathDepth, scale);
  SCALE_PARAM(mPlayer->mSwimParams.mWaitSinkSpeed, factor);
  SCALE_PARAM(mPlayer->mHangFenceParams.mMoveSp, factor);
  SCALE_PARAM(mPlayer->mHangFenceParams.mDescentSp, factor);
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
  SCALE_PARAM(mPlayer->mDirtyParams.mSlipRunSp, factor);
  SCALE_PARAM(mPlayer->mDirtyParams.mSlipCatchSp, factor);

#undef SCALE_PARAM
}

void TPlayerData::setPlayer(TMario *player) {
  mPlayer = player;
  mDefaultAttrs.recordFrom(player);
}

bool TPlayerData::loadPrm(const char *prm = "/sme.prm") {
  JKRArchive *archive = JKRFileLoader::getVolume("mario");
  SME_DEBUG_ASSERT(archive, "Mario archive could not be located!");

  if (!mParams)
    mParams = new TPlayerParams();

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

TPlayerData::ParamHistory::ParamHistory(TMario *player)
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
      mSlipYoshiParams("/Mario/SlipParamYoshi.prm"), mUpperBodyParams() {
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
