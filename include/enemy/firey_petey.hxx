#pragma once

#include "types.h"
#include <SMS/Enemy/BossPakkun.hxx>
#include <SMS/Enemy/SpineBase.hxx>
#include <SMS/Enemy/Kukku.hxx>


#define NUM_GOOP_DROPS 12

class TNerveFPSleep : public TNerveBase<TLiveActor>
{
public:
    TNerveFPSleep(){};
    virtual ~TNerveFPSleep();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPBreakSleep : public TNerveBase<TLiveActor>
{
public:
    TNerveFPBreakSleep(){};
    virtual ~TNerveFPBreakSleep();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};


class TNerveFPWait : public TNerveBPWait
{

public:
    TNerveFPWait(){};
    virtual ~TNerveFPWait(){};
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};
class TNerveFPTakeOff : public TNerveBPTakeOff
{
public:
    TNerveFPTakeOff(){};
    virtual ~TNerveFPTakeOff();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPFly : public TNerveBPFly
{
public:
    TNerveFPFly(){};
    virtual ~TNerveFPFly();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPHover : public TNerveBPHover
{
public:
    TNerveFPHover(){};
    virtual ~TNerveFPHover();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPFireBreath : public TNerveBase<TLiveActor>
{
public:
    TNerveFPFireBreath(){};
    virtual ~TNerveFPFireBreath();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TFPTornado : public TBPTornado
{
public:
    TFPTornado(TBossPakkun *parent, const char *);
    virtual ~TFPTornado(){};
    virtual void perform(u32, JDrama::TGraphics *) override;
};

class TFPFire : public THitActor {
public:
    TFPFire(TBossPakkun *parent, const char *);
    virtual ~TFPFire(){};

    virtual void perform(u32, JDrama::TGraphics *) override;

    TBossPakkun *parentPakkun;       // 0x0068
    TVec3f mVelocity; // 0x006c
    int mLifetime;
};

class TFireyPetey : public TBossPakkun
{
public:

    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef* instantiate() {
        return new TFireyPetey("TDarknessEffect");
    }

    TFireyPetey(const char *);
    virtual ~TFireyPetey();
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual void init(TLiveManager *) override;
    virtual bool receiveMessage(THitActor *, u32) override;

    short numTornados;
    TMActorKeeper *mMActorKeeperSecondary;
    TKukkuBall *mKukkuBall[NUM_GOOP_DROPS];
    TFPFire *mFire[10];
};

class TFireyPeteyManager : public TBossPakkunManager
{
public:
    TFireyPeteyManager(char const *,int);
    virtual ~TFireyPeteyManager(){};

    virtual void load(JSUMemoryInputStream &);
};