#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>

class TCannonBox : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TCannonBox("TCannonBox");
    }

    TCannonBox(const char *name);
    ~TCannonBox() override = default;

    void loadAfter() override;
    void control() override;
    bool receiveMessage(THitActor *, u32) override;

    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

    void calc() override;

protected:
    void playCollectAnim();
};

extern ObjData cannonBoxData;
