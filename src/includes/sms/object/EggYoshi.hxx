#pragma once

#include "types.h"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/actor/MActor.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/mapobj/MapObjGeneral.hxx"

class TMario;

class TEggYoshi : public TMapObjGeneral
{
public:
    TEggYoshi(const char *);
    virtual ~TEggYoshi();

    virtual void load(JSUMemoryInputStream &) override;
    virtual void control() override;
    virtual u32 getHitObjNumMax() override;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual bool receiveMessage(THitActor *, u32) override;
    virtual void touchActor(THitActor *) override;

    u32 decideRandomLoveFruit();
    void startFruit();
    void touchFruit();

    MActor *mActor;    //0x0148
    u32 mWantedFruit;  //0x014C
    TMario *mMario;    //0x0150
};