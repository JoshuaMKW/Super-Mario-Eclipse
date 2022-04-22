
#include "enemy/DarkZhine.hxx"
#include "JDrama/JDRFlagT.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;
using namespace SME::Util;

#if SME_ZHINE_BOSS

#pragma region ModelInfo

static const char *zhine_bastable[]{
    "/scene/zhine/bas/zhine_fly.bas", "/scene/zhine/bas/zhine_teleport.bas",
    "/scene/zhine/bas/zhine_spin.bas", "/scene/zhine/bas/zhine_pound.bas"};

static const TModelDataLoadEntry sZhineModelDatas[]{
    {.mModelPath = "zhine_body.bmd", .mModelFlags = 0x10300000}};

#pragma endregion ModelInfo

#pragma region Nerves

static const TNerveBZWait sNerveWait;
static const TNerveBZAppear sNerveAppear;
static const TNerveBZSleep sNerveSleep;
static const TNerveBZFly sNerveFly;
static const TNerveBZDrop sNerveDrop;
static const TNerveBZPound sNervePound;
static const TNerveBZRoll sNerveRoll;

bool TNerveBZWait::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  zhine->mActionState = TBossDarkZhine::IDLE;

  if (zhine->getClosestPlayer()) {
    zhine->mSize.set(0.3f, 0.3f, 0.3f);
    spine->mNerveStack.push(&sNerveAppear);
    return true;
  }

  zhine->mSize.set(0.0f, 0.0f, 0.0f);
  return false;
}

bool TNerveBZAppear::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  zhine->mActionState = TBossDarkZhine::IDLE;

  zhine->mStateFlags.asFlags.mIsObjDead = false;

  if (spine->mNerveTimer == 0) {
    gpMarDirector->fireStartDemoCamera("zhine_appear_camera.bck", nullptr, -1,
                                       0.0f, true, nullptr, 0, nullptr,
                                       JDrama::TFlagT<u16>(0));
    zhine->mActorData->setBck("zhine_appear");
  } else if (zhine->checkCurAnmEnd(0)) {
    gpMarDirector->fireEndDemoCamera();
    spine->mNerveStack.push(&sNerveSleep);
    spine->mNerveStack.push(&sNerveFly);
    return true;
  }

  return false;
}

bool TNerveBZSleep::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  zhine->mActionState = TBossDarkZhine::IDLE;

  if (zhine->getClosestPlayer()) {
    spine->mNerveStack.push(&sNerveSleep);
    spine->mNerveStack.push(&sNerveFly);
    return true;
  }

  zhine->sleep();
  return false;
}

bool TNerveBZFly::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  zhine->mActionState = TBossDarkZhine::FLYING;

  if (!zhine->getClosestPlayer())
    return true;

  if (zhine->advanceFlying(spine->mNerveTimer)) {
    spine->mNerveStack.push(&sNerveDrop);
    return true;
  }

  return false;
}

bool TNerveBZDrop::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  auto *params = reinterpret_cast<TBossDarkZhineParams *>(zhine->getSaveParam());
  zhine->mActionState = TBossDarkZhine::DROPPING;

  if (!zhine->getClosestPlayer())
    return true;

  if (zhine->advanceDropAttack(spine->mNerveTimer)) {
    spine->mNerveStack.push(&sNervePound);
    return true;
  }
  
  return false;
}

bool TNerveBZPound::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  auto params = reinterpret_cast<TBossDarkZhineParams *>(zhine->getSaveParam());

  if (spine->mNerveTimer == 0) {
    gpCameraShake->startShake(EnumCamShakeMode::UnknownShake27, 1.0f);
    gpPad1RumbleMgr->start(8, zhine->mPosition);
    gpPollution->stamp(1, zhine->mPosition.x, zhine->mGroundY, zhine->mPosition.z,
                        params->mSLStampRadius.get());
    MSoundSE::startSoundActor(6158, zhine->mPosition, 0, 0, 0, 4);

  } else if (spine->mNerveTimer < 80) {
    for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
      TMario *player = TGlobals::getPlayerByIndex(i);

      const bool isPlayerStateReady =
          !(player->mState & TMario::STATE_AIRBORN) &&
          (player->mState != TMario::STATE_KNCK_LND &&
            player->mPrevState != TMario::STATE_KNCK_LND);

      if (zhine->isInRange(player->mPosition, params->mSLShockRadius.get()) &&
          isPlayerStateReady) {
        if (player->mState != TMario::STATE_KNCK_LND) {
          decHP__6TMarioFi(player, 1);
          changePlayerStatus__6TMarioFUlUlb(player, TMario::STATE_KNCK_LND, 0,
                                            0);
        }
      }
    }
  } else if (spine->mNerveTimer >= params->mSLShockingTimerMax.get()) {
    spine->mNerveStack.push(&sNerveRoll);
    return true;
  }
  
  return false;
}

bool TNerveBZRoll::execute(TSpineBase<TLiveActor> *spine) const {
  auto *zhine = reinterpret_cast<TBossDarkZhine *>(spine->mTarget);
  zhine->mActionState = TBossDarkZhine::GROUNDROLL;

  if (!zhine->getClosestPlayer())
    return true;

  if (zhine->advanceRollAttack(spine->mNerveTimer)) {
    spine->mNerveStack.push(&sNerveFly);
    return true;
  }
  
  return false;
}

#pragma endregion Nerves

#pragma region Manager

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
  gpResourceManager->load("/scene/zhine/jpa/ms_zhine_osen.jpa", 0x13A);
  gpResourceManager->load("/scene/zhine/jpa/ms_zhine_wash.jpa", 0x13B);
}

#pragma endregion Manager

#pragma region DarkZhine

TBossDarkZhine::TBossDarkZhine(const char *name)
    : TSpineEnemy(name), mActionState(TBossDarkZhine::IDLE), mTarget(nullptr),
      mPolDrops(4, false), mIsFollowMario(false), mIsPounding(false),
      mIsGooping(false), mIsShocking(false), mHasIntroHappened(false) {}

TBossDarkZhine::~TBossDarkZhine() {}

const char **TBossDarkZhine::getBasNameTable() const { return zhine_bastable; }

#pragma region Init

void TBossDarkZhine::load(JSUMemoryInputStream &stream) {
  TSpineEnemy::load(stream);
  JSU_READ_PARAM(mBoundingAreaRadius, stream);
  JSU_READ_PARAM(mGoopCoverage, stream);
}

void TBossDarkZhine::loadAfter() {
  mBoundingPoint = mPosition;
  mTarget = gpMarioAddress;
}

void TBossDarkZhine::init(TLiveManager *manager) {
  mLiveManager = manager;
  manager->manageActor(this);

  mActorKeeper = new TMActorKeeper(manager, 15);
  mActorData = mActorKeeper->createMActor("zhine_body.bmd", 0);

  mActorData->calc();
  mActorData->setLightType(1);

  initHitActor(0x8000005, 5, 0x81000000, 360.0f, 360.0f, 360.0f, 360.0f);
  initAnmSound();

  mObjectType |= 1;

  for (u32 i = 0; i < mPolDrops.capacity(); ++i) {
    TBGPolDrop *polDrop = new TBGPolDrop("<TBGPolDrop>");
    polDrop->mActor = mActorKeeper->createMActor("zhine_osenball.bmd", 0);
    polDrop->mActorWhite =
        mActorKeeper->createMActor("zhine_osenball_white.bmd", 0);
    mPolDrops.push(polDrop);
  }

  J3DModel *model = mActorData->mModel;
  if (!model->mSkinDeform) {
    J3DSkinDeform *skinDeform = new J3DSkinDeform();
    model->setSkinDeform(skinDeform, J3DDeformAttachFlag::DEFORM_UNK_1);
  }

  mStateFlags.asU32 &= ~0x100;
  mActorData->offMakeDL();
  mStateFlags.asU32 |= 8;

  updateGoopKColor();

  ResTIMG *timg = reinterpret_cast<ResTIMG *>(
      JKRFileLoader::getGlbResource("/scene/map/pollution/H_ma_rak.bti"));

  if (timg) {
    SMS_ChangeTextureAll__FP12J3DModelDataPCcRC7ResTIMG(
        mActorData->mModel->mModelData, "H_ma_rak_dummy", timg);
  }

  mSize.set(0.3f, 0.3f, 0.3f);
  mSpineBase->pushNerve(&sNerveWait);
}

void TBossDarkZhine::reset() {
  mActionState = TBossDarkZhine::IDLE;

  calcRootMatrix();
  mActorData->mModel->calc();
}

#pragma endregion Init

TMario *TBossDarkZhine::getClosestPlayer() const {
  TMario *closest = nullptr;
  f32 closestDist = mBoundingAreaRadius;
  for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
    TMario *player = TGlobals::getPlayerByIndex(i);
    if (!player)
      continue;

    const f32 dist = PSVECDistance(mBoundingPoint, player->mPosition);
    if (dist < closestDist) {
      closest = player;
      closestDist = dist;
    }
  }
  SME_FUNCTION_LOG("Player = %X\n", closest);
  return closest;
}

bool TBossDarkZhine::isInRange(const TVec3f &pos, f32 r) const {
  return PSVECDistance(mBoundingPoint, pos) < r;
}

#pragma region Actions

bool TBossDarkZhine::advanceRollAttack(s32 frame) {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  if (frame < params->mSLRollingTimerMax.get()) {
    gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 400.0f);
    mRotation.y +=
        (MsWrap_f(Math::Vector3::getYAngleTo(mTarget->mPosition, mPosition),
                  mRotation.y - 180.0f, mRotation.y + 180.0f) -
         mRotation.y) *
        0.02f;
    mForwardSpeed += 1.0f;
    mForwardSpeed *= params->mSLAccelerationRate.get();
  } else {
    mForwardSpeed *= params->mSLAccelerationRate.get();
    if (mForwardSpeed < __FLT_EPSILON__) {
      mForwardSpeed = 0.0f;
      return true;
    }
  }

  const f32 yDiff = mPosition.y - mGroundY;
  if (yDiff < 30.0f) {
    mPosition.y = mGroundY;
    return false;
  } else if (yDiff > 200.0f) {
    mForwardSpeed = 0.0f;
    return true;
  }

  gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z, 60.0f);
  addGoopCoverage(0.001f);
  return false;
}

bool TBossDarkZhine::advanceDropAttack(s32 frame) {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  if (mActionState == TBossDarkZhine::DROPPING) {
    if (mPosition.y < mGroundY + 1.0f) {
      gpPollution->stamp(1, mPosition.x, mPosition.y, mPosition.z,
                         params->mSLStampRadius.get());
      addGoopCoverage(0.1f);
      gpMarioParticleManager->emitAndBindToPosPtr(67, &mPosition, 0, nullptr);
      gpMarioParticleManager->emitAndBindToPosPtr(68, &mPosition, 0, nullptr);
      gpMarioParticleManager->emitAndBindToPosPtr(69, &mPosition, 0, nullptr);
      gpMarioParticleManager->emitAndBindToPosPtr(70, &mPosition, 0, nullptr);
      mGravity = 1.0f;
      return true;
    } else {
      mGravity = 1.0f * params->mSLSpeedMultiplier.get();
      mSpeed.x = 0.0f;
      mSpeed.z = 0.0f;
    }
  }
  return false;
}

bool TBossDarkZhine::advanceFlying(s32 frame) {
  constexpr s32 FlyingTimer = 1000;

  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  if (frame < FlyingTimer) {
    if (frame % 400 == 0 || mWarpingTimer != 0) {
      TVec3f target(mTarget->mPosition.x,
                    mTarget->mFloorBelow + params->mSLMaxPoundingHeight.get(),
                    mTarget->mPosition.z);
      warpToPoint(target);
    }
    mRotation.y +=
        (MsWrap_f(Math::Vector3::getYAngleTo(mTarget->mPosition, mPosition),
                  mRotation.y - 180.0f, mRotation.y + 180.0f) -
         mRotation.y) *
        0.05f;
    if (frame % 70 == 0) {
      const TVec3f vel(mSpeed.x, mSpeed.y / 2, mSpeed.z);
      TBGPolDrop *polDrop = mPolDrops.next();
      polDrop->launch(mPosition, vel);
    }
  } else {
    mForwardSpeed *= 0.98f;
    if (mForwardSpeed < __FLT_EPSILON__) {
      mGravity = 1.0f;
      return true;
    }
  }
  mGravity = 0.0f;
  mSpeed.y = 0.0f;
  return false;
}

bool TBossDarkZhine::advanceRising(s32 frame) {
  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  mGravity = 0.0f;
  if (checkCurAnmEnd(0)) {
    TVec3f target(mTarget->mPosition.x,
                  mTarget->mFloorBelow + params->mSLMaxPoundingHeight.get(),
                  mTarget->mPosition.z);
    return warpToPoint(target);
  }
  return false;
}

bool TBossDarkZhine::warpToPoint(const TVec3f &point) {
  constexpr u32 FlashSfxID = 10387;

  mGravity = 0.0f;

  if (mWarpingTimer == 0) {
    mSize.set(0.0f, 0.0f, 0.0f);
    JAISound *sound =
        MSoundSE::startSoundActor(FlashSfxID, mPosition, 0, nullptr, 0, 4);
    if (sound)
      sound->setTempoProportion(5.0f, 20);
  } else if (mWarpingTimer > 20) {
    mPosition.set(point);
    mSize.set(0.3f, 0.3f, 0.3f);
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

#pragma endregion Actions

void TBossDarkZhine::addGoopCoverage(f32 amount) {
  mGoopCoverage = Max(Min(mGoopCoverage + amount, 1.0f), 0.0f);
}

void TBossDarkZhine::updateGoopKColor() {
  J3DModelData *modelData = mActorData->mModel->mModelData;
  {
    J3DGXColor goopColor{0, 0, 0, static_cast<u8>(mGoopCoverage * 255)};
    modelData->mStages[0]->mTevBlock->setTevKColor(
        0, goopColor); // Lerp the goop tev stage
  }
}

#pragma region Perform

bool TBossDarkZhine::receiveMessage(THitActor *actor, u32 message) {
  if (message == 15) {
    addGoopCoverage(-0.01);
    gpMarioParticleManager->emitAndBindToPosPtr(0x13B, &actor->mPosition, 1,
                                                nullptr);
  }
  return true;
}

void TBossDarkZhine::control() {
  if (mWarpingTimer > 0) {
    // mPromiEffect = gpMarioParticleManager->emitAndBindToMtxPtr(
    //     296, mActorData->mModel->mBaseMtx, 1, nullptr);
    // mBowEffect = gpMarioParticleManager->emitAndBindToMtxPtr(
    //     295, mActorData->mModel->mBaseMtx, 1, nullptr);
    mKiraEffect = gpMarioParticleManager->emitAndBindToMtxPtr(
        297, mActorData->mModel->mBaseMtx, 1, nullptr);
    // mSenkoEffect = gpMarioParticleManager->emitAndBindToMtxPtr(
    //     298, mActorData->mModel->mBaseMtx, 1, nullptr);
    // mPromiEffect->mSize2.set(4.0f, 4.0f, 4.0f);
    // mBowEffect->mSize2.set(4.0f, 4.0f, 4.0f);
    mKiraEffect->mSize2.set(12.0f, 12.0f, 12.0f);
    // mSenkoEffect->mSize2.set(4.0f, 4.0f, 4.0f);
  } else {
    // if (mPromiEffect) {
    //   mPromiEffect->deleteAllParticle();
    //   mPromiEffect = nullptr;
    // }
    // if (mKiraEffect) {
    //   mKiraEffect->deleteAllParticle();
    //   mKiraEffect = nullptr;
    // }
    // if (mBowEffect) {
    //   mBowEffect->deleteAllParticle();
    //   mBowEffect = nullptr;
    // }
    // if (mSenkoEffect) {
    //   mSenkoEffect->deleteAllParticle();
    //   mSenkoEffect = nullptr;
    // }
  }
  TSpineEnemy::control();
}

void TBossDarkZhine::moveObject() {
  TSpineEnemy::moveObject();

  auto params = reinterpret_cast<TBossDarkZhineParams *>(getSaveParam());

  switch (mActionState) {
  case TBossDarkZhine::DROPPING: {
    break;
  }
  case TBossDarkZhine::SHOCKING: {
    break;
  }
  case TBossDarkZhine::GROUNDROLL: {
    break;
  }
  case TBossDarkZhine::RISING: {
    break;
  }
  case TBossDarkZhine::FLYING: {
    break;
  }
  case TBossDarkZhine::IDLE:
  default: {
    break;
  }
  };
  mSpeed.x = mForwardSpeed * sinf(Math::angleToRadians(mRotation.y));
  mSpeed.z = mForwardSpeed * cosf(Math::angleToRadians(mRotation.y));

  for (u32 i = 0; i < mNumObjs; ++i) {
    THitActor *actor = mCollidingObjs[i];
    if (actor->mObjectID != OBJECT_ID_MARIO)
      continue;

    TMario *player = reinterpret_cast<TMario *>(actor);
    player->receiveMessage(this, 15);
  }
}

void TBossDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
  updateGoopKColor();
  for (u32 i = 0; i < mPolDrops.capacity(); ++i) {
    // mPolDrops.at(i, true)->perform(flags, graphics);
  }
  TSpineEnemy::perform(flags, graphics);
}

#pragma endregion Perform

#pragma endregion DarkZhine

#endif