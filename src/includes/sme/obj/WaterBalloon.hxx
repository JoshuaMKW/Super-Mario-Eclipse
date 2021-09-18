#include "sms/mapobj/MapObjGeneral.hxx"
#include "types.h"

#if 0

class TWaterBalloon : public TMapObjGeneral {
public:
  TWaterBalloon(const char *name);
  virtual ~TWaterBalloon();

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

  constexpr f32 getBlastRadius() const { return 100.0f; }

  void blast();
};

#endif