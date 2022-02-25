#include "GX.h"
#include "types.h"

#include "J3D/J3DModel.hxx"
#include "J3D/J3DModelLoaderDataBase.hxx"
#include "sms/M3DUtil/MActor.hxx"
#include "sms/M3DUtil/MActorKeeper.hxx"
#include "sms/manager/ModelWaterManager.hxx"
#include "sms/mapobj/MapObjBall.hxx"
#include "sms/mapobj/MapObjInit.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "SME.hxx"
#include "obj/WaterBalloon.hxx"

TWaterEmitInfo *TWaterBalloon::sEmitInfo = nullptr;

TWaterBalloon::TWaterBalloon(const char *name)
    : TMapObjBall(name), mIsExplosive(false) {
}

void TWaterBalloon::init(TLiveManager *manager) {
  mLiveManager = manager;
  mLiveManager->manageObj(this);
}

void TWaterBalloon::initActorData() {
  TMapObjBase::initActorData();

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
  }
}

bool TWaterBalloon::receiveMessage(THitActor *actor, u32 message) {
  return TMapObjBall::receiveMessage(actor, message);
}

void TWaterBalloon::control() { TMapObjBall::control(); }

void TWaterBalloon::kill() { TMapObjBall::kill(); }

void TWaterBalloon::appear() { TMapObjBall::appear(); }

void TWaterBalloon::touchActor(THitActor *actor) {
  TMapObjBase::touchActor(actor);
  blast();
}

s32 TWaterBalloon::getLivingTime() const { return 0x7FFFFFFF; }
s32 TWaterBalloon::getFlushTime() const { return 0x7FFFFFFF; }

void TWaterBalloon::hold(TTakeActor *actor) {
  TMapObjBall::hold(actor);
  mIsExplosive = true;
}
void TWaterBalloon::put() {
  TMapObjBall::put();
  mIsExplosive = false;
}

void TWaterBalloon::thrown() { TMapObjBall::thrown(); }

void TWaterBalloon::touchGround(JGeometry::TVec3<f32> *pos) {
  if (PSVECMag(reinterpret_cast<Vec *>(&mSpeed)) >= 20.0f || mSpeed.y < -10.0f)
    blast();
  else {
    TMapObjBall::touchGround(pos);
  }
}

void TWaterBalloon::touchWall(JGeometry::TVec3<f32> *pos,
                              TBGWallCheckRecord *record) {
  blast();
}

void TWaterBalloon::touchPollution()  {
  blast();
}


void TWaterBalloon::touchWaterSurface() {
  blast();
}

void TWaterBalloon::touchWater(THitActor *actor) {
  TMapObjBall::touchWater(actor);
}

void TWaterBalloon::touchRoof(JGeometry::TVec3<f32> *pos) { blast(); }

void TWaterBalloon::kicked() {
  blast();
}

void TWaterBalloon::blast() {
  TWaterEmitInfo emitInfo = *sEmitInfo;

  JGeometry::TVec3<f32> pos(mPosition);
  pos.y += 100.0f;

  emitInfo.mPos.set(pos);
  emitInfo.mV.set(JGeometry::TVec3<f32>{0.0f, -10.0f, 0.0f});
  emitInfo.mSize.set(80.0f);
  emitInfo.mNum.set(1);

  sEmitInfo->mPos.set(mPosition);

  gpModelWaterManager->emitRequest(*sEmitInfo);
  gpModelWaterManager->emitRequest(emitInfo);
  makeObjDead();
}

static hit_data waterBalloon_hit_data{
    .mAttackRadius = 60.0f, .mAttackHeight = 60.0f, .mReceiveRadius = 60.0f, .mReceiveHeight = 60.0f};

static obj_hit_info waterBalloon_collision_data{._00 = 1,
                                         .mType = 0xDC000000,
                                         ._08 = 0,
                                         .mHitData = &waterBalloon_hit_data};

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

static sound_info waterBalloon_sound_info{.mLength = 10,
                                   .mSoundData = &waterBalloon_sound_data};

static ObjPhysicalData waterBalloon_physical_data{.mGravity = 0.3f,
                                           .mFloorBounceSpeed = 0.3f,
                                           .mWallBounceSpeed = 0.1f,
                                           ._0C = 0.1f,
                                           .mFloorBrakeFactor = 0.9f,
                                           ._14 = 0.97f,
                                           .mAirBrakeFactor = 1.0f,
                                           ._1C = {5.0f, 0.5f, 0.7f, 0.0f},
                                           .mThrowDistance = 2.8f,
                                           .mThrowHeight = 16.0f};

static ObjPhysicalInfo waterBalloon_physical_info{
    ._00 = 13, .mPhysicalData = &waterBalloon_physical_data, ._0C = 2};

ObjData waterBalloonData{
    .mMdlName = "waterballoon",
    .mObjectID = 0x80000FFF,
    .mLiveManagerName =
        sLiveManagerName, // const_cast<char *>("木マネージャー")
    .mObjKey = nullptr,   // const_cast<char *>("waterballoon"),
    ._10 = 0,
    .mObjCollisionData = &waterBalloon_collision_data,
    .mMapCollisionInfo = nullptr,
    .mSoundInfo = &waterBalloon_sound_info,
    .mPhysicalInfo = &waterBalloon_physical_info,
    .mSinkData = nullptr,
    ._28 = nullptr,
    .mBckMoveData = nullptr,
    ._30 = 50.0f,
    .mUnkFlags = 0x02130100,
    .mKeyCode = SME::Util::cexp_calcKeyCode("waterballoon")};