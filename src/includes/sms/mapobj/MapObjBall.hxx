#pragma once

#include "sms/mapobj/MapObjGeneral.hxx"
#include "types.h"

class TMapObjBall : public TMapObjGeneral {
public:
  TMapObjBall(const char *);
  virtual ~TMapObjBall();

  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void control() override;
  virtual void makeObjAppeared() override;
  virtual void initMapObj() override;
  virtual void touchActor(THitActor *) override;
  virtual void touchWater(THitActor *) override;
  virtual void makeObjDefault() override;
  virtual f32 getDepthAtFloating() override;
  virtual s32 getLivingTime() const override;
  virtual void appearing() override;
  virtual void breaking() override;
  virtual void waitingToAppear() override;
  virtual void hold(TTakeActor *) override;
  virtual void put() override;
  virtual void touchGround(JGeometry::TVec3<f32> *) override;
  virtual void checkWallCollision(JGeometry::TVec3<f32> *) override;
  virtual void touchWall(JGeometry::TVec3<f32> *, TBGWallCheckRecord *) override;
  virtual void touchRoof(JGeometry::TVec3<f32> *) override;
  virtual void rebound(JGeometry::TVec3<f32> *);
  virtual void touchWaterSurface();
  virtual void touchPollution();
  virtual void kicked();
  virtual void calcCurrentMtx();

  void boundByActor(THitActor *);

  f32 _148;
  f32 _14C;
  f32 _150;
  f32 _154;
  f32 _158;
  f32 _15C;
  f32 _160;
  f32 _164;
  f32 _168;
  f32 _16C;
  f32 _170;
  f32 _174;
  f32 mKickedVSpeed;
  f32 _178;
  f32 _17C;
  f32 _180;
  f32 _184;
  f32 _188;
  f32 _18C;
  f32 mAdditionalHeldHeight;
  u32 _194;
  f32 _198;
};