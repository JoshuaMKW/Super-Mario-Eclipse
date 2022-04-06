#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRNameRef.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGeometry.hxx"
#include "sms/actor/LiveActor.hxx"
#include "sms/graph/GraphWeb.hxx"
#include "sms/manager/LiveManager.hxx"
#include "sms/params/BaseParam.hxx"
#include "sms/params/Params.hxx"
#include "types.h"


class TConductor : public JDrama::TNameRef {
public:
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  class TConductorParams : public TParams {
    TConductorParams(const char *prm)
        : CONSTRUCT_PARAM(mEnemyFarClip, 5000.0f),
          CONSTRUCT_PARAM(mGenerateRadiusMax, 1000.0f),
          CONSTRUCT_PARAM(mGenerateRadiusMin, 200.0f),
          CONSTRUCT_PARAM(mGenerateTime, 300),
          CONSTRUCT_PARAM(mGenerateProp, 0.01f) {
      load(prm);
    }

    TParamT<f32> mEnemyFarClip;
    TParamT<f32> mGenerateRadiusMax;
    TParamT<f32> mGenerateRadiusMin;
    TParamT<s32> mGenerateTime;
    TParamT<f32> mGenerateProp;
  };
#undef CONSTRUCT_PARAM

  struct TConductorPacket {
    size_t _00;
    void *_04;
    void *_08;
    void *_0C;
  };

  TConductor();
  virtual ~TConductor();

  virtual JDrama::TNameRef *searchF(u16, const char *) override;
  virtual void perform(u32, JDrama::TGraphics *);

  void genEnemyFromPollution();
  TGraphWeb *getGraphByName(const char *name);
  TLiveManager *getManagerByName(const char *name);
  void init();
  void initGraphGroup();
  bool isBossDefeated();
  void killEnemiesWithin(const TVec3f &pos, f32 radius);
  void makeEnemyAppear(const TVec3f &pos, const char *name, int,
                       int);
  void makeGraphGroup(void *);
  void makeOneEnemyAppear(const TVec3f &pos, const char *name,
                          int);
  void registerAloneActor(TLiveActor *actor);
  void registerAreaCylinderManager(void * /*TAreaCylinderManager*/);
  void registerEnemyInfoTable(void * /*TStageEnemyInfoTable*/);
  void registerEnemyManager(void * /*TEnemyManager*/);
  void registerGenerator(void * /*TGenerator*/);
  void registerManager(TLiveManager *);
  void registerOtherObj(JDrama::TViewObj *);
  void registerSDLModelData(void * /*SDLModelData*/);

  TConductorPacket _10;
  TConductorPacket _20;
  TConductorPacket _30;
  TConductorPacket _40;
  TConductorPacket _50;
  TConductorPacket _60;
  TConductorPacket _70;
  u32 _80;
  TConductorParams mParams;
  u32 _F0;
  void *mNpcParams; // TNpcParams *
  u32 _F8;
  u32 _FC;
};
