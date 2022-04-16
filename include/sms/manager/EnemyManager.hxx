#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "sms/manager/ObjManager.hxx"
#include "sms/spc/SpcBinary.hxx"
#include "LiveManager.hxx"

class TLiveActor;

class TEnemyManager : public TLiveManager {
public:
  TEnemyManager(char const *);
  virtual ~TEnemyManager();

  virtual void load(JSUMemoryInputStream &);
  virtual void perform(u32, JDrama::TGraphics *);
  virtual bool createEnemyInstance();
  virtual void clipEnemies(JDrama::TGraphics *);
  virtual void restoreDrawBuffer(u32);
  virtual void createEnemies(int);
  virtual void changeDrawBuffer(u32);

  
  u32 _38; // 0
  f32 _3c; // 1.0f
  u32 _40; // 0
  u32 _44; // 0
  u32 _48; // 0
  u32 _4c; // -1
  u32 _50; // 0
};