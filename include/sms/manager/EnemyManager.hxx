#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "sms/manager/ObjManager.hxx"
#include "sms/spc/SpcBinary.hxx"
#include "LiveManager.hxx"

class TLiveActor;

class TSharedMActorSet {
  MActor **mActors;
  size_t mActorNum;
  s32 mCurAnmIdx;
};

class TEnemyManager : public TLiveManager {
public:
  TEnemyManager(char const *);
  virtual ~TEnemyManager();

  virtual void load(JSUMemoryInputStream &);
  virtual void perform(u32, JDrama::TGraphics *);
  virtual TSpineEnemy *createEnemyInstance();
  virtual void clipEnemies(JDrama::TGraphics *);
  virtual void restoreDrawBuffer(u32);
  virtual void createEnemies(int);
  virtual void changeDrawBuffer(u32);

  void copyAnmMtx(TSpineEnemy *enemy);
  void copyFromShared();
  s32 countLivingEnemy() const;
  void createSharedMActorSet(const char **models);
  TSpineEnemy *getDeadEnemy();
  TSpineEnemy *getFarOutEnemy();
  TSpineEnemy *getNearestEnemy(const TVec3f &pos);
  TParams *getSaveParam() const;
  TSharedMActorSet *getSharedMActorSet(int);
  void killChildren();
  void killChildrenWithin(const TVec3f &pos, f32 radius);
  void performShared(u32, JDrama::TGraphics *);
  void setSharedFlags();
  void updateAnmSoundShared();
  
  TParams *mEnemyParams; // 0
  f32 _3C; // 1.0f
  u32 _40; // 0
  u32 _44; // 0
  u32 _48; // 0
  u32 _4c; // -1
  u32 _50; // 0
};