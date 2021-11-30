#include "sms/mapobj/MapObjBall.hxx"
#include "sms/mapobj/MapObjInit.hxx"
#include "sms/nozzle/Watergun.hxx"
#include "types.h"


class TWaterBalloon : public TMapObjBall {
public:
  TWaterBalloon(const char *name);
  virtual ~TWaterBalloon();

  virtual void init(TLiveManager *) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void control() override;
  virtual void kill() override;
  virtual void appear() override;
  virtual void touchActor(THitActor *actor) override;
  virtual s32 getLivingTime() const override;
  virtual s32 getFlushTime() const override;
  virtual void hold(TTakeActor *) override;
  virtual void put() override;
  virtual void thrown() override;
  virtual void touchGround(JGeometry::TVec3<f32> *) override;
  virtual void touchWall(JGeometry::TVec3<f32> *,
                         TBGWallCheckRecord *) override;
  virtual void touchRoof(JGeometry::TVec3<f32> *) override;
  virtual void touchPollution() override;
  virtual void touchWaterSurface() override;
  virtual void kicked() override;

  void initActorData();
  void blast();

  bool mIsExplosive;
  static TWaterEmitInfo *sEmitInfo;
};

extern ObjData waterBalloonData;