#include "GX.h"
#include "MTX.h"
#include "types.h"

#include "J3D/J3DModel.hxx"
#include "J3D/J3DModelLoaderDataBase.hxx"
#include "sms/M3DUtil/MActor.hxx"
#include "sms/M3DUtil/MActorKeeper.hxx"
#include "sms/manager/ModelWaterManager.hxx"
#include "sms/mapobj/MapObjBall.hxx"
#include "sms/mapobj/MapObjInit.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "sms/sound/MSoundSESystem.hxx"
#include "sms/collision/BGCheck.hxx"

#include "SME.hxx"
#include "obj/WaterBalloon.hxx"

TWaterEmitInfo *TWaterBalloon::sEmitInfo = nullptr;
constexpr f32 MaxSpeed = 30.0f;

TWaterBalloon::TWaterBalloon(const char *name)
    : TMapObjBall(name), mIsPopped(false) {}

void TWaterBalloon::init(TLiveManager *manager) {
  mLiveManager = manager;
  mLiveManager->manageObj(this);
}

void TWaterBalloon::initActorData() {
  TMapObjBall::initActorData();

  /*
  MActorAnmData *anmData = new MActorAnmData();
  anmData->init("/mario/waterballoon", nullptr);

  MActor *actor = new MActor(anmData);

  void *resource =
  JKRFileLoader::getGlbResource("/mario/waterballoon/waterballoon.bmd");
  J3DModelData *modelData =
      J3DModelLoaderDataBase::load(resource, 0);

  J3DModel *model = new J3DModel(modelData, 0, 1);
  actor->setModel(model, 0);

  mActorData = actor;
  */
}

TWaterBalloon::~TWaterBalloon() {}

void TWaterBalloon::perform(u32 flags, JDrama::TGraphics *graphics) {
  TMapObjBall::perform(flags, graphics);
  if (flags & PERFORM_ON_MOVEMENT) {
    mForwardSpeed = PSVECMag(reinterpret_cast<Vec *>(&mSpeed));
  }
}

bool TWaterBalloon::receiveMessage(THitActor *actor, u32 message) {
  return TMapObjBall::receiveMessage(actor, message);
}

void TWaterBalloon::control() {
  TMapObjBall::control();
  if (mForwardSpeed > MaxSpeed) {
    TVec3f dir;
    getReflectionDir(*mFloorBelow->getNormal(), dir);

    blast(dir);
  }
}

void TWaterBalloon::kill() { TMapObjBall::kill(); }

void TWaterBalloon::appear() {
  TMapObjBall::appear();
  mIsPopped = false;
}

s32 TWaterBalloon::getLivingTime() const { return 0x7FFFFFFF; }
s32 TWaterBalloon::getFlushTime() const { return 0; }

void TWaterBalloon::hold(TTakeActor *actor) { TMapObjBall::hold(actor); }
void TWaterBalloon::put() { TMapObjBall::put(); }

void TWaterBalloon::thrown() { TMapObjBall::thrown(); }

void TWaterBalloon::touchActor(THitActor *actor) {
  if (actor->mObjectID != 0x80000001) { // Not Mario
    blast({0.0f, 1.0f, 0.0f});
    return;
  }

  TMario *player = reinterpret_cast<TMario *>(actor);
  if (player->mState == 0x820008AB)
    return; // Throwing

  if (mForwardSpeed >= MaxSpeed) {
    blast({0.0f, 1.0f, 0.0f});
    return;
  } else if (PSVECMag(reinterpret_cast<Vec *>(&player->mSpeed)) >= MaxSpeed) {
    blast({0.0f, 1.0f, 0.0f});
    return;
  }

  const f32 pangle = static_cast<f32>(player->mAngle.y / 182);
  const f32 speed = player->mForwardSpeed + 5.0f;

  TVec3f dir(0.0f, 0.0f, 0.0f);
  dir.x = sinf(Math::angleToRadians(pangle));
  dir.z = cosf(Math::angleToRadians(pangle));
  dir.scale(speed);

  SME_LOG("speed = { %.02f, %.02f, %.02f }\n", dir.x, dir.y, dir.z);
  mSpeed.add(dir);

  if (gpMSound->gateCheck(0x1807)) {
    MSoundSESystem::MSoundSE::startSoundActor(
        0x1807, reinterpret_cast<Vec *>(&mPosition), 0, nullptr, 0, 4);
  }
}

void TWaterBalloon::touchGround(TVec3f *pos) {
  if (mFloorBelow->mCollisionType == 2048) {
    TMapObjBall::touchGround(pos);
    return;
  }

  if (!(mForwardSpeed >= MaxSpeed || mSpeed.y < -10.0f)) {
    TMapObjBall::touchGround(pos);
    return;
  }

  TVec3f dir;
  getReflectionDir(*mFloorBelow->getNormal(), dir);

  blast(dir);
}

void TWaterBalloon::touchWall(TVec3f *pos,
                              TBGWallCheckRecord *record) {
  TVec3f dir;
  getReflectionDir(*mWallTouching->getNormal(), dir);

  blast(dir);
}

void TWaterBalloon::touchPollution() {
  TVec3f dir;
  getReflectionDir(*mFloorBelow->getNormal(), dir);

  blast(dir);
}

void TWaterBalloon::touchWaterSurface() {
  TVec3f dir;
  getReflectionDir({0.0f, 1.0f, 0.0f}, dir);

  blast(dir);
}

void TWaterBalloon::touchWater(THitActor *actor) {
  TMapObjBall::touchWater(actor);
}

void TWaterBalloon::touchRoof(TVec3f *pos) {
  TVec3f dir;
  getReflectionDir(*mRoofTouching->getNormal(), dir);

  blast(dir);
}

void TWaterBalloon::kicked() { TMapObjBall::kicked(); }

void TWaterBalloon::blast(TVec3f blastSpd) {
  if (mHolder && mHolder->mObjectID == 0x80000001)
    receiveMessage(mHolder, 7); // Drop from Mario

  TWaterEmitInfo emitInfo = *sEmitInfo;

  TVec3f pos(mPosition);
  pos.y += 100.0f;

  TVec3f vel(blastSpd);
  PSVECNormalize(reinterpret_cast<Vec *>(&vel), reinterpret_cast<Vec *>(&vel));
  vel.scale(7.0f + PSVECMag(reinterpret_cast<Vec *>(&blastSpd)) * 0.1f);

  emitInfo.mV.set(vel);
  emitInfo.mSize.set(emitInfo.mSize.get() * 2);
  emitInfo.mPos.set(mPosition);
  emitInfo.mHitRadius.set(emitInfo.mHitRadius.get() * 10);
  gpModelWaterManager->emitRequest(emitInfo);

  emitInfo = *sEmitInfo;
  vel.y -= 5.0f;
  emitInfo.mPos.set(pos);
  emitInfo.mV.set(vel);
  emitInfo.mSize.set(80.0f);
  emitInfo.mHitRadius.set(150.0f);
  emitInfo.mNum.set(3);

  sEmitInfo->mPos.set(mPosition);

  gpModelWaterManager->emitRequest(emitInfo);


  if (gpMSound->gateCheck(0x28B8)) {
    MSoundSESystem::MSoundSE::startSoundActor(
        0x28B8, reinterpret_cast<Vec *>(&mPosition), 0, nullptr, 0, 4);
  }

  makeObjDead();
  mIsPopped = true;
}

void TWaterBalloon::getReflectionDir(
    const TVec3f &reflectionNormal,
    TVec3f &out) const {
  f32 dot2 =
      2 * PSVECDotProduct(reinterpret_cast<const Vec *>(&mSpeed),
                          reinterpret_cast<const Vec *>(&reflectionNormal));
  TVec3f rvec(0.0f, 0.0f, 0.0f);
  rvec.scale(dot2, reflectionNormal);
  out.sub(mSpeed, rvec);
}

static hit_data waterBalloon_hit_data{.mAttackRadius = 40.0f,
                                      .mAttackHeight = 40.0f,
                                      .mReceiveRadius = 50.0f,
                                      .mReceiveHeight = 50.0f};
// static hit_data waterBalloon_hit_data{.mAttackRadius = 150.0f,
//                                       .mAttackHeight = 60.0f,
//                                       .mReceiveRadius = 60.0f,
//                                       .mReceiveHeight = 90.0f};

static obj_hit_info waterBalloon_collision_data{._00 = 1,
                                                .mType = 0xDC000000,
                                                ._08 = 0,
                                                .mHitData =
                                                    &waterBalloon_hit_data};

static sound_data waterBalloon_sound_data{
    ._00 = 0xFFFFFFFF,
    ._04 = 0x3801,
    ._08 = 0xFFFFFFFF,
    .mUnkSoundID = 0x1807,
    .mReboundSoundID = 0xFFFFFFFF,
    ._14 = 0xFFFFFFFF,
    ._18 = 0xFFFFFFFF,
    ._1C = 0xFFFFFFFF,
    ._20 = 0xFFFFFFFF,
    ._24 = 0xFFFFFFFF,
};

static sound_info waterBalloon_sound_info{
    .mLength = 10, .mSoundData = &waterBalloon_sound_data};

static ObjPhysicalData waterBalloon_physical_data{.mGravity = 0.3f,
                                                  .mFloorBounceSpeed = 0.3f,
                                                  .mWallBounceSpeed = 0.1f,
                                                  .mRotationStopFactor = 0.1f,
                                                  .mFloorBrakeFactor = 0.9f,
                                                  .mRollBrakeFactor = 0.97f,
                                                  .mAirBrakeFactor = 1.0f,
                                                  .mRollBrakeFactor2 = 1.0f,
                                                  ._20 = {0.0f, 0.7f, 0.6f},
                                                  .mThrowDistance = 2.8f,
                                                  .mThrowHeight = 16.0f};

static ObjPhysicalInfo waterBalloon_physical_info{
    ._00 = 13, .mPhysicalData = &waterBalloon_physical_data, ._0C = 2};

ObjData waterBalloonData{
    .mMdlName = "waterballoon",
    .mObjectID = 0x40000FFF /*0x80000FFF*/,
    .mLiveManagerName =
        gLiveManagerName,       // const_cast<char *>("木マネージャー")
    .mObjKey = gUnkManagerName, // const_cast<char *>("waterballoon"),
    .mAnimInfo = nullptr,
    .mObjCollisionData = &waterBalloon_collision_data,
    .mMapCollisionInfo = nullptr,
    .mSoundInfo = &waterBalloon_sound_info,
    .mPhysicalInfo = &waterBalloon_physical_info,
    .mSinkData = nullptr,
    ._28 = nullptr,
    .mBckMoveData = nullptr,
    ._30 = 50.0f,
    .mUnkFlags = 0x170100 /*0x02130100*/,
    .mKeyCode = SME::Util::cexp_calcKeyCode("waterballoon")};