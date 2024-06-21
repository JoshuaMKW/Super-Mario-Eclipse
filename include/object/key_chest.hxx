#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>
class TKeyChest : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TKeyChest("TKeyChest");
    }

    TKeyChest(const char *name);
    virtual ~TKeyChest();
    virtual void kill() override;
    virtual void control() override;
    virtual void touchPlayer(THitActor *) override;
    virtual void playIdleAnim();

   
    TMario *mFollowActor;
    
};

extern ObjData keyChestData;
