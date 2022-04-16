
#include "enemy/DarkZhine.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;
using namespace SME::Util;

#if SME_ZHINE_BOSS

static const char *zhine_bastable[]{
    "/scene/zhine/bas/zhine_fly.bas", "/scene/zhine/bas/zhine_teleport.bas",
    "/scene/zhine/bas/zhine_spin.bas", "/scene/zhine/bas/zhine_pound.bas"};

static const TModelDataLoadEntry sZhineModelDatas[]{
    {.mModelPath = "zhine_body.bmd", .mModelFlags = 0x10300000}};

TBossDarkZhineManager::TBossDarkZhineManager(const char *name)
    : TEnemyManager(name){};
TBossDarkZhineManager::~TBossDarkZhineManager() {
  TEnemyManager::~TEnemyManager();
}

void TBossDarkZhineManager::load(JSUMemoryInputStream &stream) {
  mEnemyParams = new TBossDarkZhineParams("/enemy/bosszhine.prm");
  TEnemyManager::load(stream);
  initJParticle();
}

void TBossDarkZhineManager::createModelData() {
  TObjManager::createModelDataArray(sZhineModelDatas);
}

void TBossDarkZhineManager::initJParticle() {
  // gpResourceManager->load("/scene/zhine/jpa/ms_boge_wash.jpa", 0x13B);
}

TBossDarkZhine::TBossDarkZhine(const char *name)
    : TSpineEnemy(name), mActionState(TBossDarkZhine::RISING), mTarget(nullptr),
      mPolDrops(4, false), mIsFollowMario(false), mIsPounding(false),
      mIsGooping(false), mIsShocking(false) {}

TBossDarkZhine::~TBossDarkZhine() {}

const char **TBossDarkZhine::getBasNameTable() const { return zhine_bastable; }

void TBossDarkZhine::load(JSUMemoryInputStream &stream) {
  TSpineEnemy::load(stream);
  JSU_READ_PARAM(mBoundingAreaRadius, stream);
  JSU_READ_PARAM(mGoopCoverage, stream);
}

void TBossDarkZhine::init(TLiveManager *manager) {
  mLiveManager = manager;
  manager->manageActor(this);

  mMActorKeeper = new TMActorKeeper(manager, 15);
  mActorData = mMActorKeeper->createMActor("zhine_body.bmd", 0);

  mActorData->calc();
  mActorData->setLightType(1);

  mBoundingPoint = mPosition;

  initHitActor(0x8000105, 5, 0x81000000, 60.0f, 60.0f, 60.0f, 60.0f);
  initAnmSound();

  mObjectType |= 1;

  for (u32 i = 0; i < mPolDrops.capacity(); ++i) {
    mPolDrops.push(new TBGPolDrop("<TBGPolDrop>"));
  }

  J3DModel *model = mActorData->mModel;
  if (!model->mSkinDeform) {
    J3DSkinDeform *skinDeform = new J3DSkinDeform();
    model->setSkinDeform(skinDeform, J3DDeformAttachFlag::UNK_1);
  }

  mStateFlags.asU32 &= ~0x100;
  mActorData->offMakeDL();
  mStateFlags.asU32 |= 8;

  J3DModelData *modelData = mActorData->mModel->mModelData;
  {
    J3DGXColor goopColor{0, 0, 0, static_cast<u8>(mGoopCoverage * 255)};
    modelData->mStages[0]->mTevBlock->setTevKColor(
        0, goopColor); // Lerp the goop tev stage
  }

  ResTIMG *timg = reinterpret_cast<ResTIMG *>(
      JKRFileLoader::getGlbResource("/scene/map/pollution/H_ma_rak.bti"));

  if (timg) {
    SMS_ChangeTextureAll__FP12J3DModelDataPCcRC7ResTIMG(
        mActorData->mModel->mModelData, "H_ma_rak_dummy", timg);
  }
}

void TBossDarkZhine::reset() {
  mActionState = TBossDarkZhine::IDLE;

  calcRootMatrix();
  mActorData->mModel->calc();
}

bool TBossDarkZhine::receiveMessage(THitActor *actor, u32 message) {
  if (message == 15) {
    mGoopCoverage -= 0.01f;
  }
  return true;
}

f32 TBossDarkZhine::getAngleToTarget() const {
  return atan2f(mTarget->mPosition.x - mPosition.x,
                mTarget->mPosition.z - mPosition.z);
}

bool TBossDarkZhine::isInRange(const TVec3f &pos, f32 r) const {
  return PSVECDistance(mBoundingPoint, pos) < r;
}

bool TBossDarkZhine::advanceRollAttack() {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  if (mStatusTimer < params->mRollingTimerMax.get()) {
    gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 400.0f);
    mForwardSpeed += 1.0f;
    mForwardSpeed *= 0.98f;
    mStatusTimer += 1;
  } else {
    mForwardSpeed *= 0.98f;
    if (mForwardSpeed < __FLT_EPSILON__) {
      mActionState = TBossDarkZhine::RISING;
      mStatusTimer = 0;
      return true;
    }
  }
  gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 60.0f);
  mGoopCoverage += 0.001f;
  return false;
}

bool TBossDarkZhine::advanceDropAttack() {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  if (mActionState == TBossDarkZhine::DROPPING) {
    if (belongToGround() && mPosition.y < mGroundY + 1.0f) {
      gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z,
                         params->mStampRadius.get());
      mGoopCoverage += 0.1f;
      mGravity = 1.0f;
      mActionState = TBossDarkZhine::SHOCKING;
      mStatusTimer = 0;
      return true;
    } else {
      mGravity = 1.0f * params->mSpeedMultiplier.get();
      mSpeed.x = 0.0f;
      mSpeed.z = 0.0f;
      return false;
    }
  } else if (mActionState == TBossDarkZhine::SHOCKING) {
    if (mStatusTimer < params->mShockingTimerMax.get()) {
      for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
        TMario *player = TGlobals::getPlayerByIndex(i);

        const bool isPlayerStateReady =
            !(player->mState & TMario::STATE_AIRBORN) &&
            (player->mState != TMario::STATE_KNCK_LND &&
             player->mState != 0x4045C);

        if (isInRange(player->mPosition, params->mShockRadius.get()) &&
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

bool TBossDarkZhine::advanceFlying() {
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
      mActionState = TBossDarkZhine::DROPPING;
      mStatusTimer = 0;
      mGravity = 1.0f;
      return true;
    }
  }
  mGravity = 0.0f;
  return false;
}

bool TBossDarkZhine::advanceRising() {
  if (checkCurAnmEnd(0)) {
    TVec3f target(mTarget->mPosition.x, mTarget->mFloorBelow + 800.0f,
                  mTarget->mPosition.z);
    warpToPoint(target);
  }
  return false;
}

bool TBossDarkZhine::warpToPoint(const TVec3f &point) {
  constexpr u32 FlashEffectID = 161;
  constexpr u32 ShockEffectID = 162;
  constexpr u32 FlashSfxID = 10387;

  if (mWarpingTimer == 0) {
    gpMarioParticleManager->emit(FlashEffectID, &mPosition, 1, nullptr);
    mSize.set(0.0f, 0.0f, 0.0f);
    JAISound *sound =
        MSoundSE::startSoundActor(FlashSfxID, mPosition, 0, nullptr, 0, 4);
    sound->setTempoProportion(2.0f, 20);
  } else if (mWarpingTimer > 20) {
    mPosition.set(point);
    mSize.set(1.0f, 1.0f, 1.0f);
    gpMarioParticleManager->emit(FlashEffectID, &mPosition, 1, nullptr);
    mWarpingTimer = 0;
    return true;
  }
  mWarpingTimer += 1;
  return false;
}

void TBossDarkZhine::sleep() {
  if (PSVECDistance(mPosition, mBoundingPoint) > __FLT_EPSILON__) {
    warpToPoint(mBoundingPoint);
  }
  // mActorData->setBckFromIndex(1);
  // mActorData->setFrameRate(SMSGetAnmFrameRate__Fv() * 0.5, 0);
  mSpeed.set(0.0f, 0.0f, 0.0f);
  mGravity = 0.0f;
}

void TBossDarkZhine::control() { TSpineEnemy::control(); }

void TBossDarkZhine::moveObject() {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  bool advanced = false;
  switch (mActionState) {
  case TBossDarkZhine::DROPPING: {
    advanced = advanceDropAttack();
    if (advanced) {
      gpCameraShake->startShake(EnumCamShakeMode::UnknownShake27, 1.0f);
      gpPad1RumbleMgr->start(8, mPosition);
      gpPollution->stamp(1, mPosition.x, mGroundY, mPosition.z,
                         params->mStampRadius.get());
      MSoundSE::startSoundActor(6158, mPosition, 0, 0, 0, 4);
    }
    break;
  }
  case TBossDarkZhine::SHOCKING: {
    advanced = advanceDropAttack();
    if (advanced) {
      // mActorData->setBckFromIndex(2);
    }
    break;
  }
  case TBossDarkZhine::GROUNDROLL: {
    advanced = advanceRollAttack();
    if (advanced) {
      // mActorData->setBckFromIndex(1);
    }
    break;
  }
  case TBossDarkZhine::RISING: {
    advanced = advanceRising();
    if (advanced) {
      // mActorData->setBckFromIndex(0);
    }
    break;
  }
  case TBossDarkZhine::FLYING: {
    advanced = advanceFlying();
    if (advanced) {
    }
    break;
  }
  case TBossDarkZhine::IDLE:
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

  bool targetWithin = false;

  for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
    TMario *player = TGlobals::getPlayerByIndex(i);
    if (!player)
      continue;

    if (!(PSVECDistance(mBoundingPoint, player->mPosition) >
          mBoundingAreaRadius)) {
      targetWithin = true;
      break;
    }
  }

  if (advanced && !targetWithin) {
    mActionState = TBossDarkZhine::IDLE;
  } else if (!targetWithin && mActionState == TBossDarkZhine::IDLE) {
    mActionState = TBossDarkZhine::RISING;
  }
}

void TBossDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
  J3DModelData *modelData = mActorData->mModel->mModelData;
  {
    J3DGXColor goopColor{0, 0, 0, static_cast<u8>(mGoopCoverage * 255)};
    modelData->mStages[0]->mTevBlock->setTevKColor(
        0, goopColor); // Lerp the goop tev stage
  }

  TSpineEnemy::perform(flags, graphics);
}

#endif