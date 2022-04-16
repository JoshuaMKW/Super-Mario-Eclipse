#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/SpineEnemy.hxx"
#include "sms/actor/HitActor.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRActor.hxx"
#include "sms/actor/SpineBase.hxx"
#include "sms/manager/EnemyManager.hxx"
#include "JGeometry.hxx"

class TBPTornado;
class TBPPolDrop;
class TBossPakkun : public TSpineEnemy
{
public:
    TBossPakkun(const char *);
    virtual ~TBossPakkun();

    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual bool receiveMessage(THitActor *, u32) override;
    virtual void init(TLiveManager *) override;
    virtual void setGroundCollision() override;
    virtual void kill() override;
    virtual void *getBasNameTable() const override;

    void changeBck(int);
    void launchPolDrop();
    void gotHipDropDamage();
    void showMessage(u32);
    void rumblePad(int, const JGeometry::TVec3<float> &);
    void checkMarioRiding();
    f32 _14C;
    f32 _00;                    // 0x0154
    TBPPolDrop *mPollutionDrop; // 0x0158
    void *TBPVomit;             // 0x015C
    TBPTornado *mTornado;       // 0x0160
    void *TNavel;               // 0x0164
    void *THeadHit;             // 0x0168
    s8 _02;                     // 0x016C
    u32 _03[0x18 / 3];          // 0x0170
    s8 _04;                     // 0x017C
    MActor *peteyMActorData;    // 0x0180
    f32 _06;                    // 0x0184
    u32 _07;                    // 0x0188
    void *TWaterEmitInfo;       // 0x018C
    s8 _08;                     // 0x0190
    u32 _0c[6];                 // 0x01B8
    u32 _09;                    // 0x01B8
    s8 _10;                     // 0x01BC
    u32 _11;                    // 0x01C0
    s8 _12;                     // 0x01C4
    u32 _13;                    // 0x01CC
};

class TNerveBPWait : public TNerveBase<TLiveActor>
{
public:
    TNerveBPWait(){};
    virtual ~TNerveBPWait();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
    static TNerveBase<TLiveActor> *theNerve();
};
class TNerveBPTakeOff : public TNerveBase<TLiveActor>
{
public:
    TNerveBPTakeOff(){};
    virtual ~TNerveBPTakeOff();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
    static TNerveBase<TLiveActor> *theNerve();
};

class TNerveBPFly : public TNerveBase<TLiveActor>
{
public:
    TNerveBPFly(){};
    TNerveBPFly(const TNerveBPFly &a){};
    virtual ~TNerveBPFly();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
    static TNerveBase<TLiveActor> *theNerve();
};

class TNerveBPHover : public TNerveBase<TLiveActor>
{
public:
    TNerveBPHover(){};
    virtual ~TNerveBPHover();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
    static TNerveBase<TLiveActor> *theNerve();
};

class TBPPolDrop : public THitActor
{

public:
    TBPPolDrop(TBossPakkun *parentPakkun, char *name);
    virtual void perform(u32, JDrama::TGraphics *) override;
    void move();
    TBossPakkun *parentPakkun;       // 0x0068
    JGeometry::TVec3<f32> mVelocity; // 0x006c
    MActor *mActorData;              // 0x0078
    u32 _01[3];                      // 0x007c-84
    f32 _02;                         // 0x0088
};

class TBPTornado : public THitActor
{
public:
    TBPTornado(TBossPakkun *parent, const char *);
    virtual ~TBPTornado();
    virtual void perform(u32, JDrama::TGraphics *) override;
    TBossPakkun *mParent; // 0x68
    u32 _6C[0x28 / 4];    // TMActorKeeper
    f32 _94;              // 0x94
    u32 _98;              // 0x98
};

class TBossPakkunManager : public TEnemyManager
{
public:
    TBossPakkunManager(const char *,int isDemoMode);
    virtual ~TBossPakkunManager();

    virtual void load(JSUMemoryInputStream &);
    virtual void createModelData();

    u32 isDemoMode;
};