#pragma once

#include "types.h"

#include "JDrama/JDRGraphics.hxx"
#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/M3DUtil/MActor.hxx"
#include "sms/M3DUtil/MActorKeeper.hxx"
#include "sms/actor/RidingInfo.hxx"
#include "sms/actor/SpineBase.hxx"
#include "sms/actor/TakeActor.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/m3d/LodAnm.hxx"
#include "sms/manager/LiveManager.hxx"
#include "sms/sound/MAnmSound.hxx"

class TMapCollisionManager;

class TLiveActor : public TTakeActor {

public:
  TLiveActor(const char *);
  virtual ~TLiveActor();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual Mtx44 *getTakingMtx() override;
  virtual bool belongToGround() const;
  virtual Mtx44 *getRootJointMtx() const;
  virtual void init(TLiveManager *);
  virtual void calcRootMatrix();
  virtual void setGroundCollision();
  virtual void control();
  virtual void bind();
  virtual void moveObject();
  virtual void requestShadow();
  virtual void drawObject(JDrama::TGraphics *);
  virtual void performOnlyDraw(u32, JDrama::TGraphics *);
  virtual u32 getShadowType();
  virtual void kill();
  virtual f32 getGravityY() const;
  virtual bool hasMapCollision() const;
  virtual void getFocalPoint() const;
  virtual void updateAnmSound();
  virtual const char **getBasNameTable() const;

  MActor *getMActor() const;
  void stopAnmSound();
  void setCurAnmSound();
  void setAnmSound(const char *);
  void initAnmSound();
  void getJointTransByIndex(int, TVec3f *) const;
  void calcVelocityToJumpToY(const TVec3f &, f32, f32) const;
  void initLodAnm(const TLodAnmIndex *, int, f32);
  J3DModel *getModel() const;
  void calcRideMomentum();
  void calcRidePos();

  TLiveManager *mLiveManager; // 0x0070
  MActor *mActorData;         // 0x0074
  TMActorKeeper *mActorKeeper;
  u16 mActorIndex;            // _7C
  MAnmSound *mAnmSound;       // _80
  char *mBasName;             // _84
  u32 *_88;
  TSpineBase<TLiveActor> *mSpineBase;        // 0x008C
  u32 _90;                                   // 0x0090
  TVec3f mPositionalVelocity; // 0x0094
  TVec3f mRotationalVelocity; // 0x00A0
  TVec3f mSpeed;              // 0x00AC
  f32 mShadowRadius;                         // 0x00B8
  f32 mMaxSpeed;                             // 0x00BC
  f32 _C0;                                   // 0x00C0
  TBGCheckData *mFloorBelow;                 // 0x00C4
  f32 mGroundY;                              // 0x00C8
  f32 mGravity;                              // 0x00CC
  u32 _D0;
  TRidingInfo mRidingInfo; // _D4
  f32 _E4;
  u8 _E8;
  TMapCollisionManager *mColMgr; // _EC

  union {
    u32 asU32;
    struct {
      u32 _00 : 7;
      bool mIsSunken : 1;
      u32 _01 : 1;
      bool mIsRide : 1;
      u32 _02 : 1;
      bool mCanBeTaken : 1;
      u32 _03 : 2;
      bool mCanTalk : 1;
      u32 _04 : 10;
      bool mHasPhysics : 1;
      u32 _05 : 1;
      bool mClipFromScene : 1;
      u32 _06 : 3;
      bool mIsObjDead : 1;
    } asFlags;
  } mStateFlags;
};