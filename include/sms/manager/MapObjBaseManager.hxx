#include "sms/mapobj/MapObjBase.hxx"
#include "LiveManager.hxx"
#include "types.h"

class TMapObjBaseManager : public TLiveManager {
public:
  TMapObjBaseManager(const char *);
  virtual ~TMapObjBaseManager();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void clipActors(JDrama::TGraphics *) override;
  virtual void createModelData() override;

  u32 getObjNumWithActorType(u32) const;
  void makeObjAppear(f32, f32, f32, u32, bool);
  void makeObjAppear(u32);
  void makeObjAppeared(u32);
  
  static u32 getActorTypeByEventID(u32);
  static TMapObjBase *newAndRegisterObj(const char *, const JGeometry::TVec3<f32> &,
                                 const JGeometry::TVec3<f32> &,
                                 const JGeometry::TVec3<f32> &);
  static TMapObjBase *newAndRegisterObjByEventID(u32, const char *);

  f32 _38;
  f32 _3C;
};