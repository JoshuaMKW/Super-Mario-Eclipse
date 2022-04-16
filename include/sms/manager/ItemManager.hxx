#include "MapObjBaseManager.hxx"
#include "sms/actor/Item.hxx"
#include "types.h"


class TItemManager : public TMapObjBaseManager {
public:
  TItemManager(const char *);
  virtual ~TItemManager();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool hasMapCollision() const override;

  void makeShineAppearWithDemo(const char *, const char *, f32, f32, f32);
  void makeShineAppearWithDemoOffset(const char *, const char *, f32, f32, f32);
  void makeShineAppearWithTime(const char *, int, f32, f32, f32, int, int, int);
  void resetNozzleBoxesModel(int);

  static TItem *newAndRegisterCoin(u32);
  static TItem *newAndRegisterCoinReal();
  
  f32 _40;
  f32 _44;
  f32 _48;
  f32 _4C;
  f32 _50;
  f32 _54;
  f32 _58;
  f32 _5C;
  f32 _60;
  f32 _64;
  f32 _68;
  f32 _6C;
  f32 _70;
  f32 _74;
  f32 _78;
};