#include "GX.h"
#include "types.h"

#include "J3D/J3DModel.hxx"
#include "J3D/J3DModelLoaderDataBase.hxx"
#include "sms/M3DUtil/MActor.hxx"
#include "sms/M3DUtil/MActorKeeper.hxx"
#include "sms/manager/ModelWaterManager.hxx"
#include "sms/mapobj/MapObjBall.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "SME.hxx"
#include "WaterBalloon.hxx"

TWaterEmitInfo *TWaterBalloon::sEmitInfo = nullptr;

TWaterBalloon::TWaterBalloon(const char *name)
    : TMapObjBall(name), mIsExplosive(false) {}

void TWaterBalloon::init(TLiveManager *manager) {
  mLiveManager = manager;
  mLiveManager->manageObj(this);

  sEmitInfo = new TWaterEmitInfo("/Mario/waterballoon/waterballoon.prm");
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
    sEmitInfo->mPos.set(mPosition);
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

void TWaterBalloon::touchRoof(JGeometry::TVec3<f32> *pos) { blast(); }

void TWaterBalloon::kicked() {
  blast();
}

void TWaterBalloon::blast() {
  TWaterEmitInfo emitInfo = *sEmitInfo;

  JGeometry::TVec3<f32> pos(emitInfo.mPos.get());
  pos.y += 100.0f;

  emitInfo.mPos.set(pos);
  emitInfo.mV.set(JGeometry::TVec3<f32>{0.0f, -10.0f, 0.0f});
  emitInfo.mSize.set(80.0f);
  emitInfo.mNum.set(1);

  gpModelWaterManager->emitRequest(*sEmitInfo);
  gpModelWaterManager->emitRequest(emitInfo);
  makeObjDead();
}