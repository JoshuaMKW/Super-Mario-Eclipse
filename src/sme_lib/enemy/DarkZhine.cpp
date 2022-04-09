
#include "enemy/DarkZhine.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;
using namespace SME::Util;

#if SME_ZHINE_BOSS
/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

static const char *zhine_bastable[]{
    "/scene/zhine/bas/zhine_fly.bas", "/scene/zhine/bas/zhine_teleport.bas",
    "/scene/zhine/bas/zhine_spin.bas", "/scene/zhine/bas/zhine_pound.bas"};

TDarkZhine::TDarkZhine(const char *name)
    : TSpineEnemy(name), mActionState(TDarkZhine::RISING), mTarget(nullptr),
      mPolDrops(4, false), mIsFollowMario(false), mIsPounding(false),
      mIsGooping(false), mIsShocking(false), mParams("/Zhine/Zhine.prm") {}

TDarkZhine::~TDarkZhine() {}

const char **TDarkZhine::getBasNameTable() const { return zhine_bastable; }

void TDarkZhine::load(JSUMemoryInputStream &stream) {
  TSpineEnemy::load(stream);
}

void TDarkZhine::init(TLiveManager *manager) {
  mLiveManager = manager;
  manager->manageActor(this);

  mMActorKeeper = new TMActorKeeper(manager, 15);
  mMActorKeeper->createMActor("zhine_body.bmd", 0);

  mActorData->calc();
  mActorData->setLightType(1);

  initHitActor(0x8000005, 5, 0x81000000, 60.0f, 60.0f, 60.0f, 60.0f);

  for (u32 i = 0; i < mPolDrops.capacity(); ++i) {
    mPolDrops.push(new TBGPolDrop("<TBGPolDrop>"));
  }
}

bool TDarkZhine::receiveMessage(THitActor *actor, u32 message) {
  if (message == 15) {
    mGoopCoverage -= 0.01f;
  }
  return true;
}

f32 TDarkZhine::getAngleToTarget() const {
  return atan2f(mTarget->mPosition.x - mPosition.x,
                mTarget->mPosition.z - mPosition.z);
}

bool TDarkZhine::isInRange(const TVec3f &pos, f32 r) const {
  return PSVECDistance(mBoundingPoint, pos) < r;
}

bool TDarkZhine::advanceRollAttack() {
  if (mStatusTimer < mParams.mRollingTimerMax.get()) {
    gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 400.0f);
    mForwardSpeed += 1.0f;
    mForwardSpeed *= 0.98f;
    mStatusTimer += 1;
  } else {
    mForwardSpeed *= 0.98f;
    if (mForwardSpeed < __FLT_EPSILON__) {
      mActionState = TDarkZhine::RISING;
      mStatusTimer = 0;
      return true;
    }
  }
  gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 60.0f);
  mGoopCoverage += 0.001f;
  return false;
}

bool TDarkZhine::advanceDropAttack() {
  if (mActionState == TDarkZhine::DROPPING) {
    if (belongToGround() && mPosition.y < mGroundY + 1.0f) {
      gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z,
                         mParams.mStampRadius.get());
      mGoopCoverage += 0.1f;
      mGravity = 1.0f;
      mActionState = TDarkZhine::SHOCKING;
      mStatusTimer = 0;
      return true;
    } else {
      mGravity = 1.0f * mParams.mSpeedMultiplier.get();
      mSpeed.x = 0.0f;
      mSpeed.z = 0.0f;
      return false;
    }
  } else if (mActionState == TDarkZhine::SHOCKING) {
    if (mStatusTimer < mParams.mShockingTimerMax.get()) {
      for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
        TMario *player = TGlobals::getPlayerByIndex(i);

        const bool isPlayerStateReady =
            !(player->mState & TMario::STATE_AIRBORN) &&
            (player->mState != TMario::STATE_KNCK_LND &&
             player->mState != 0x4045C);

        if (isInRange(player->mPosition, mParams.mShockRadius.get()) &&
            isPlayerStateReady) {
          decHP__6TMarioFi(player, 1);
          changePlayerStatus__6TMarioFUlUlb(player, TMario::STATE_KNCK_LND, 0,
                                            0);
        }
      }
    }
  }
  return false;
}

bool TDarkZhine::advanceFlying() {
  constexpr s32 FlyingTimer = 500;

  if (mStatusTimer < FlyingTimer) {
    if (mStatusTimer % 150) {
      // teleport
    }
    mRotation.y +=
        Math::Vector3::getYAngleTo(mPosition, mTarget->mPosition) * 0.1f;
    if (mStatusTimer % 20) {
      const TVec3f vel(mSpeed.x, mSpeed.y / 2, mSpeed.z);
      TBGPolDrop *polDrop = mPolDrops.next();
      polDrop->launch(mPosition, vel);
    }
  } else {
    mForwardSpeed *= 0.98f;
    if (mForwardSpeed < __FLT_EPSILON__) {
      mActionState = TDarkZhine::DROPPING;
      mStatusTimer = 0;
      return true;
    }
  }
  return false;
}

bool TDarkZhine::advanceRising() {
  if (checkCurAnmEnd(0)) {
    TVec3f target(mTarget->mPosition.x, mTarget->mFloorBelow + 800.0f,
                  mTarget->mPosition.z);
    warpToPoint(target);
  }
  return false;
}

bool TDarkZhine::warpToPoint(const TVec3f &point) {
  constexpr u32 FlashEffectID = 161;
  constexpr u32 ShockEffectID = 162;
  constexpr u32 FlashSfxID = 10387;

  if (mWarpingTimer == 0) {
    gpMarioParticleManager->emit(
        FlashEffectID, &mPosition, 1, nullptr);
    mSize.set(0.0f, 0.0f, 0.0f);
    JAISound *sound = MSoundSE::startSoundActor(FlashSfxID, mPosition, 0, nullptr, 0, 4);
    sound->setTempoProportion(2.0f, 20);
  } else if (mWarpingTimer > 20) {
    mPosition.set(point);
    mSize.set(1.0f, 1.0f, 1.0f);
    gpMarioParticleManager->emit(
        FlashEffectID, &mPosition, 1, nullptr);
    mWarpingTimer = 0;
    return true;
  }
  mWarpingTimer += 1;
  return false;
}

void TDarkZhine::moveObject() {
  bool advanced = false;
  switch (mActionState) {
  case TDarkZhine::DROPPING: {
    advanced = advanceDropAttack();
    if (advanced) {
      gpCameraShake->startShake(EnumCamShakeMode::UnknownShake27, 1.0f);
      gpPad1RumbleMgr->start(8, mPosition);
      gpPollution->stamp(1, mPosition.x, mGroundY, mPosition.z,
                         mParams.mStampRadius.get());
      MSoundSE::startSoundActor(6158, mPosition, 0, 0, 0, 4);
    }
    break;
  }
  case TDarkZhine::SHOCKING: {
    advanced = advanceDropAttack();
    if (advanced) {
      // mActorData->setBckFromIndex(2);
    }
    break;
  }
  case TDarkZhine::GROUNDROLL: {
    advanced = advanceRollAttack();
    if (advanced) {
      // mActorData->setBckFromIndex(1);
    }
    break;
  }
  case TDarkZhine::RISING: {
    advanced = advanceRising();
    if (advanced) {
      // mActorData->setBckFromIndex(0);
    }
    break;
  }
  case TDarkZhine::FLYING: {
    advanced = advanceFlying();
    if (advanced) {
    }
    break;
  }
  case TDarkZhine::IDLE:
  default: {
    sleep();
    break;
  }
  };
  mSpeed.x = mForwardSpeed * sinf(Math::angleToRadians(mRotation.y));
  mSpeed.z = mForwardSpeed * cosf(Math::angleToRadians(mRotation.y));

  for (u32 i = 0; i < mNumObjs; ++i) {
    THitActor *actor = mCollidingObjs[i];
    if (actor->mObjectID != 0x80000001)
      continue;

    TMario *player = reinterpret_cast<TMario *>(actor);
    player->receiveMessage(this, 15);
  }
}

void TDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
  TSpineEnemy::perform(flags, graphics);
}

#endif