#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/actor/TakeActor.hxx"
#include "sms/collision/MapCheck.hxx"
#include "sms/mapobj/MapObjBase.hxx"

class TMapObjGeneral : public TMapObjBase
{
public:
    TMapObjGeneral(const char *);
    virtual ~TMapObjGeneral();

    virtual void loadAfter() override;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual bool receiveMessage(THitActor *, u32) override;
    virtual void ensureTakeSituation() override;
    virtual void calcRootMatrix() override;
    virtual void control() override;
    virtual void bind() override;
    virtual void kill() override;
    virtual void appear() override;
    virtual void initMapObj() override;
    virtual void checkIllegalAttr() const override;
    virtual void touchPlayer(THitActor *) override;
    virtual void touchWater(THitActor *) override;
    virtual s32 getLivingTime() const;
    virtual s32 getFlushTime() const;
    virtual bool isPollutedGround(const JGeometry::TVec3<f32> &) const;
    virtual void work();
    virtual void appearing();
    virtual void appeared();
    virtual void breaking();
    virtual void sinking();
    virtual void holding();
    virtual void waitingToRecover();
    virtual void recovering();
    virtual void waitingToAppear();
    virtual void touchingPlayer();
    virtual void touchingWater();
    virtual void hold(TTakeActor *);
    virtual void put();
    virtual void thrown();
    virtual void sink();
    virtual void recover();
    virtual void waitToAppear(s32);
    virtual void makeObjBuried();
    virtual void makeObjRecovered();
    virtual void receiveMessageFromPlayer();
    virtual void calcVelocity();
    virtual void checkGroundCollision(JGeometry::TVec3<f32> *);
    virtual void touchGround(JGeometry::TVec3<f32> *);
    virtual void checkWallCollision(JGeometry::TVec3<f32> *);
    virtual void touchWall(JGeometry::TVec3<f32> *, TBGWallCheckRecord *);
    virtual void checkRoofCollision(JGeometry::TVec3<f32> *);
    virtual void touchRoof(JGeometry::TVec3<f32> *);

    u32 _01; //0x0138
    u32 _02; //0x013C
    f32 _03; //0x0140
    f32 _04; //0x0144
};