#pragma once

#include "types.h"
#include <SMS/Enemy/BossPakkun.hxx>
#include <SMS/Enemy/SpineBase.hxx>
#include <SMS/Enemy/Kukku.hxx>


#define NUM_GOOP_DROPS 12


class TNerveFPTumbleOut : public TNerveBase<TLiveActor>
{
public:
    TNerveFPTumbleOut() {};
    virtual ~TNerveFPTumbleOut() = default;
    virtual bool execute(TSpineBase<TLiveActor>* mSpineBase) const;
};

class TNerveFPTumble : public TNerveBase<TLiveActor>
{
public:
    TNerveFPTumble() {};
    virtual ~TNerveFPTumble() = default;
    virtual bool execute(TSpineBase<TLiveActor>* mSpineBase) const;
};

class TNerveFPTumbleIn : public TNerveBase<TLiveActor>
{
public:
    TNerveFPTumbleIn() {};
    virtual ~TNerveFPTumbleIn() = default;
    virtual bool execute(TSpineBase<TLiveActor>* mSpineBase) const;
};

class TNerveFPSwallow : public TNerveBase<TLiveActor>
{
public:
    TNerveFPSwallow() {};
    virtual ~TNerveFPSwallow() = default;
    virtual bool execute(TSpineBase<TLiveActor>* mSpineBase) const;
};


class TNerveFPFall : public TNerveBase<TLiveActor>
{
public:
    TNerveFPFall() {};
    virtual ~TNerveFPFall() = default;
    virtual bool execute(TSpineBase<TLiveActor>* mSpineBase) const;
};


class TNerveFPSleep : public TNerveBase<TLiveActor>
{
public:
    TNerveFPSleep(){};
    virtual ~TNerveFPSleep() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPBreakSleep : public TNerveBase<TLiveActor>
{
public:
    TNerveFPBreakSleep(){};
    virtual ~TNerveFPBreakSleep() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};


class TNerveFPWait : public TNerveBase<TLiveActor>
{

public:
    TNerveFPWait(){};
    virtual ~TNerveFPWait() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};
class TNerveFPTakeOff : public TNerveBPTakeOff
{
public:
    TNerveFPTakeOff(){};
    virtual ~TNerveFPTakeOff() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPFly : public TNerveBPFly
{
public:
    TNerveFPFly(){};
    virtual ~TNerveFPFly() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPHover : public TNerveBPHover
{
public:
    TNerveFPHover(){};
    virtual ~TNerveFPHover() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TNerveFPFireBreath : public TNerveBase<TLiveActor>
{
public:
    TNerveFPFireBreath(){};
    virtual ~TNerveFPFireBreath() = default;
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase) const;
};

class TFPTornado : public TBPTornado
{
public:
    TFPTornado(TBossPakkun *parent, const char *);
    virtual ~TFPTornado(){};
    virtual void perform(u32, JDrama::TGraphics *) override;

private:
    f32 mMaxTornadoHeight;
};

class TFPHeadHit : public TBPHeadHit {
public:
    TFPHeadHit(TBossPakkun* parentPakkun, const char* name) : TBPHeadHit(parentPakkun, name) {
        
    }
    virtual bool receiveMessage(THitActor* sender, u32 msg) override;
};

class TFPNavel : public TBPNavel {
public:
    TFPNavel(TBossPakkun* parentPakkun, const char* name) : TBPNavel(parentPakkun,name) {
    }
    virtual bool receiveMessage(THitActor* sender, u32 msg) override;
};


class TFPFire : public THitActor {
public:
    TFPFire(const char *);
    virtual ~TFPFire(){};

    virtual void perform(u32, JDrama::TGraphics *) override;

    TVec3f mVelocity; // 0x006c
    u32 mLifetime;
};

class TFireyPetey : public TBossPakkun
{
public:

    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef* instantiate() {
        return new TFireyPetey("FireyPetey");
    }

    TFireyPetey(const char *);
    virtual ~TFireyPetey() = default;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual void init(TLiveManager *) override;
    virtual bool receiveMessage(THitActor *, u32) override;

    void gotHipDropDamage();

    s16 mNumTornados;
    TMActorKeeper *mMActorKeeperSecondary;
    TKukkuBall *mKukkuBall[NUM_GOOP_DROPS];
    TFPFire *mFire[10];
};

class TFireyPeteyManager : public TBossPakkunManager
{
public:

    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef* instantiate() {
        return new TFireyPeteyManager("FireyPeteyManager", 0);
    }

    TFireyPeteyManager(char const *,int);
    virtual ~TFireyPeteyManager(){};

    virtual void load(JSUMemoryInputStream &);
};


void dropLava(TSpineBase<TLiveActor>* spine, TFireyPetey* petey);
void dropLavaBig(TSpineBase<TLiveActor>* spine, TFireyPetey* petey);