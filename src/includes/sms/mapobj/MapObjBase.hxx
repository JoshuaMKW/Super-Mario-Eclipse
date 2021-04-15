#pragma once

#include "types.h"
#include "GX.h"
#include "sms/JSystem/J3D/J3DModel.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/actor/LiveActor.hxx"

class TMapObjBase : public TLiveActor
{
public:
    TMapObjBase(const char *);
    virtual ~TMapObjBase();

    virtual void load(JSUMemoryInputStream &) override;
    virtual void loadAfter() override;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual bool receiveMessage(THitActor *, u32) override;
    virtual Mtx *getTakingMtx() override;
    virtual void ensureTakeSituation() override;
    virtual f32 getRadiusAtY(f32) const override;
    virtual Mtx *getRootJointMtx() const override;
    virtual void calcRootMatrix() override;
    virtual void setGroundCollision() override;
    virtual void control() override;
    virtual u32 getShadowType() override;
    virtual void kill() override;
    virtual void appear();
    virtual void makeObjAppeared();
    virtual void makeObjDead();
    virtual void changeObjSRT(const JGeometry::TVec3<f32> &,
                              const JGeometry::TVec3<f32> &,
                              const JGeometry::TVec3<f32> &);
    virtual void changeObjMtx(Mtx *);
    virtual void updateObjMtx();
    virtual void setUpCurrentMapCollision();
    virtual void setObjHitData(u16);
    virtual void setModelMtx(Mtx *);
    virtual void initMapObj();
    virtual void loadBeforeInit(JSUMemoryInputStream &);
    virtual void initMapCollisionData();
    virtual void makeMActors();
    virtual u32 getSDLModelFlag() const;
    virtual void checkIllegalAttr() const;
    virtual void calc();
    virtual void draw() const;
    virtual void dead();
    virtual void touchActor(THitActor *);
    virtual void touchPlayer(THitActor *);
    virtual void touchWater(THitActor *);
    virtual void touchEnemy(THitActor *);
    virtual void touchBoss(THitActor *);
    virtual void makeObjDefault();
    virtual u32 getHitObjNumMax();
    virtual f32 getDepthAtFloating();

    bool animIsFinished() const;
    void awake();
    void calcMap();
    void calcReflectingVelocity(const TBGCheckData *, f32,
                                JGeometry::TVec3<f32> *) const;
    void checkOnManhole();
    void concatOnlyRotFromLeft(Mtx *, Mtx *, Mtx *);
    void concatOnlyRotFromRight(Mtx *, Mtx *, Mtx *);
    void emitAndBindScale(s32, u8, const JGeometry::TVec3<f32> *,
                          const JGeometry::TVec3<f32> &) const;
    void emitAndRotateScale(s32, u8, const JGeometry::TVec3<f32> *) const;
    void emitAndScale(s32, u8, const JGeometry::TVec3<f32> *) const;
    void emitAndScale(s32, u8, const JGeometry::TVec3<f32> *,
                      const JGeometry::TVec3<f32> &) const;
    void emitAndSRT(s32, u8, const JGeometry::TVec3<f32> *,
                    const JGeometry::TVec3<f32> &,
                    const JGeometry::TVec3<f32> &);
    void emitColumnWater();
    void *getBuildingJoint(int);
    void *getBuildingJointObj(int);
    f32 getDistance(const JGeometry::TVec3<f32> &) const;
    f32 getDistanceXZ(const JGeometry::TVec3<f32> &) const;
    Vec *getNormalVecFromTarget(f32, f32, f32, JGeometry::TVec3<f32> *) const;
    Vec *getNormalVecFromTargetXZ(f32, f32, JGeometry::TVec3<f32> *) const;
    f32 getObjCollisionHeightOffset() const;
    f32 getRotYFromAxisX(const JGeometry::TVec3<f32> &) const;
    f32 getRotYFromAxisZ(const JGeometry::TVec3<f32> &) const;
    JGeometry::TVec3<f32> *getVerticalVecToTargetXZ(f32, f32,
                                                    JGeometry::TVec3<f32> *);
    bool hasAnim(u16) const;
    bool hasModelOrAnimData(u16) const;
    void initActorData();
    void initAndRegister(const char *);
    void initBckMoveData();
    void initMActor(const char *, const char *, u32);
    void initModelData();
    void initObjCollisionData();
    void initUnique();
    void makeObjMtxRotByAxis(const JGeometry::TVec3<f32> &, f32, Mtx *) const;
    void makeRootMtxRotX(Mtx *);
    void makeRootMtxRotY(Mtx *);
    void makeRootMtxRotZ(Mtx *);
    void makeVecToLocalZ(f32, JGeometry::TVec3<f32> *);
    bool marioHeadAttack() const;
    bool marioHipAttack() const;
    bool marioIsOn() const;
    void removeMapCollision();
    void sendMsg(u32, u32);
    void setRootMtxRotY();
    void setRootMtxRotZ();
    void setUpMapCollision(u16);
    void sleep();
    void soundBas(u32, f32, f32);
    void startAnim(u16);
    void startBck(const char *);
    void startControlAnim(u16);
    void startSound(u16);
    void throwObjToFront(TMapObjBase *, f32, f32) const;
    void throwObjToFrontFromPoint(TMapObjBase *, const JGeometry::TVec3<f32> &,
                                  f32, f32) const;
    void updateRootMtxTrans();

    static f32 getJointScaleY(J3DJoint *);
    static f32 getJointTransX(J3DJoint *);
    static f32 getJointTransZ(J3DJoint *);
    static s32 getWaterID(THitActor *);
    static void *getWaterPlane(THitActor *);
    static void *getWaterPos(THitActor *);
    static void *getWaterSpeed(THitActor *);
    static void initPacketColor(J3DModel *, _GXTevRegID, const _GXColorS10 *);
    static bool isCoin(THitActor *);
    static bool isDemo();
    static bool isFruit(THitActor *);
    static bool isHideObj(THitActor *);
    static void joinToGroup(const char *, THitActor *);
    static void loadHideObjInfo(JSUMemoryInputStream &,
                                s32 *, f32 *, f32 *, s32 *);
    static void makeLowerStr(const char *, char *);
    static void makeMtxRotByAxis(const JGeometry::TVec3<f32> &, f32, Mtx *);
    static bool marioIsOn(const TLiveActor *);
    static void moveJoint(J3DJoint *, f32, f32, f32);
    static void *newAndInitBuildingCollisionMove(int, TLiveActor *);
    static void *newAndInitBuildingCollisionWarp(int, TLiveActor *);
    static void rotateVecByAxisY(JGeometry::TVec3<f32>, f32);
    static void setJointScaleY(J3DJoint *, f32);
    static void setJointTransX(J3DJoint *, f32);
    static void setJointTransY(J3DJoint *, f32);
    static void setJointTransZ(J3DJoint *, f32);
    static void startAllAnim(MActor *, const char *);
    static void throwObjFromPointWithRot(TMapObjBase *, const JGeometry::TVec3<f32> &,
                                         const JGeometry::TVec3<f32> &, f32, f32);
    static bool waterHitPlane(THitActor *);

    char *mRegisterName; //?
    u16 mState;        //0x00FC
    u16 _01;           //0x00FE
    u32 _02;           //0x0100
    s32 mStateTimer;   //0x0104
    u32 _03[0x2C / 4]; //0x0108
    u32 mMapObjID;     //0x0134
};