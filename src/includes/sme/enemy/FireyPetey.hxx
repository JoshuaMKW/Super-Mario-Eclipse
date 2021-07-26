#pragma once

#include "types.h"
#include "sms/enemy/BossPakkun.hxx"



class TNerveFPSleep{
    public:
    TNerveFPSleep();
    virtual ~TNerveFPSleep();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase);
};

class TNerveFPBreakSleep{
    public:
    TNerveFPBreakSleep();
    virtual ~TNerveFPBreakSleep();
    virtual bool execute(TSpineBase<TLiveActor> *mSpineBase);
};

class TFireyPetey : public TBossPakkun
{

    public:
    TFireyPetey(const char *);
    virtual ~TFireyPetey();
    virtual void init(TLiveManager *) override;
    virtual bool receiveMessage(THitActor *,u32) override;
};


